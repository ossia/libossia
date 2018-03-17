#pragma once
#include <ossia-config.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <smallfun.hpp>
#include <readerwriterqueue.h>

#if defined(OSSIA_PROTOCOL_AUDIO)
  #if defined(__EMSCRIPTEN__)
    #define USE_SDL
  #elif defined(_MSC_VER)
    #define USE_JACK
  #else
    #define USE_PORTAUDIO
  #endif
#endif

#if defined(USE_SDL)
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#elif defined(USE_JACK)
#define USE_WEAK_JACK 1
#define NO_JACK_METADATA 1
struct __jack_port;
struct __jack_client;
typedef struct _jack_port jack_port_t;
typedef struct _jack_client jack_client_t;
typedef uint32_t jack_nframes_t;

#elif defined(USE_PORTAUDIO)
#include <portaudio.h>

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
    std::string card_in, card_out;
    int rate{44100};
    int bufferSize{512};
    int inputs{};
    int outputs{};

    std::atomic_bool replace_tick{false};
    smallfun::function<void(unsigned long, double), 256> ui_tick;
    smallfun::function<void(unsigned long, double), 256> audio_tick;

    static void process_generic(audio_protocol& self, float** inputs, float** outputs, uint64_t nsamples);
#if defined(USE_SDL)
    static int SDLCallback(void* userData,Uint8* _stream,int _length);

#elif defined(USE_JACK)

    jack_client_t *client{};
    std::vector<jack_port_t*> input_ports;
    std::vector<jack_port_t*> output_ports;
    static int process(jack_nframes_t nframes, void *arg);

#elif defined(USE_PORTAUDIO)
    static int PortAudioCallback(
        const void *input, void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);
    PaStream* stream();
    PaStream* m_stream{};
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

  private:
    ossia::net::device_base* m_dev{};

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
