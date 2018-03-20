#pragma once
#include <ossia/audio/audio_protocol.hpp>
struct __jack_port;
struct __jack_client;
typedef struct _jack_port jack_port_t;
typedef struct _jack_client jack_client_t;
typedef uint32_t jack_nframes_t;

namespace ossia
{
class jack_engine final
    : public audio_engine
{
  public:
    jack_engine();
    ~jack_engine() override;

    void reload(audio_protocol* cur) override;
    void stop() override;

  private:
    static int process(jack_nframes_t nframes, void* arg);

    std::atomic<jack_client_t*> client{};
    std::vector<jack_port_t*> input_ports;
    std::vector<jack_port_t*> output_ports;
};

}
