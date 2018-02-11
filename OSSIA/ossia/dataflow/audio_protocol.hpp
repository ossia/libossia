#pragma once
#include <ossia-config.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <smallfun.hpp>
#include <readerwriterqueue.h>

#if defined(OSSIA_PROTOCOL_AUDIO)
#if defined(__EMSCRIPTEN__)
  #include <SDL/SDL.h>
  #include <SDL/SDL_audio.h>
#else
  #include <portaudio.h>
#endif
#else
using PaStream = int;
using PaStreamCallbackTimeInfo = int;
using PaStreamCallbackFlags = int;
#endif


namespace ossia
{
class OSSIA_EXPORT audio_protocol : public ossia::net::protocol_base
{
  public:
    int rate{44100};
    int bufferSize{512};
    int inputs{};
    int outputs{};

    std::atomic_bool replace_tick{false};
    smallfun::function<void(unsigned long, double), 128> ui_tick;
    smallfun::function<void(unsigned long, double), 128> audio_tick;

#if defined(__EMSCRIPTEN__)
    static int SDLCallback(void* userData,Uint8* _stream,int _length);
#else
    static int PortAudioCallback(
        const void *input, void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);
#endif

    audio_protocol();

    ~audio_protocol() override;

    void set_tick(smallfun::function<void(unsigned long, double)> t)
    {
      ui_tick = std::move(t);
      replace_tick = true;
    }

    bool pull(ossia::net::parameter_base&) override;
    bool push(const ossia::net::parameter_base&) override;
    bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
    bool push_raw(const ossia::net::full_parameter_data&) override;
    bool push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&) override;
    bool observe(ossia::net::parameter_base&, bool) override;
    bool update(ossia::net::node_base& node_base) override;
    void set_device(ossia::net::device_base& dev) override;

    void stop() override;

    void reload();

    void register_parameter(mapped_audio_parameter& p);
    void unregister_parameter(mapped_audio_parameter& p);
    void register_parameter(virtual_audio_parameter& p);
    void unregister_parameter(virtual_audio_parameter& p);

#if !defined(__EMSCRIPTEN__)
    PaStream* stream();
#endif
  private:
    ossia::net::device_base* m_dev{};
#if !defined(__EMSCRIPTEN__)
    PaStream* m_stream{};
#endif

    ossia::audio_parameter* main_audio_in{};
    ossia::audio_parameter* main_audio_out{};
    std::vector<ossia::audio_parameter*> audio_ins;
    std::vector<ossia::audio_parameter*> audio_outs;

    std::vector<ossia::mapped_audio_parameter*> in_mappings;
    std::vector<ossia::mapped_audio_parameter*> out_mappings;
    std::vector<ossia::virtual_audio_parameter*> virtaudio;
    moodycamel::ReaderWriterQueue<smallfun::function<void()>> funlist;
};


class OSSIA_EXPORT audio_device
{
  public:
    audio_device(std::string name = "audio");
    ~audio_device();

    ossia::audio_parameter& get_main_in();
    ossia::audio_parameter& get_main_out();

    ossia::net::generic_device device;
    ossia::audio_protocol& protocol;
};

}
