#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/dataflow/audio_parameter.hpp>
#include <portaudio.h>

namespace ossia
{
class OSSIA_EXPORT audio_protocol : public ossia::net::protocol_base
{
  public:
    int rate{44100};
    int bufferSize{64};
    int inputs{};
    int outputs{};

    std::atomic_bool replace_tick{false};
    std::function<void(unsigned long)> ui_tick;
    std::function<void(unsigned long)> audio_tick;
    static int PortAudioCallback(
        const void *input, void *output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);

    audio_protocol();

    ~audio_protocol();

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

    PaStream* stream();
  private:
    ossia::net::device_base* m_dev{};
    PaStream* m_stream{};

    ossia::audio_parameter* main_audio_in{};
    ossia::audio_parameter* main_audio_out{};
    std::vector<ossia::audio_parameter*> audio_ins;
    std::vector<ossia::audio_parameter*> audio_outs;
};

}
