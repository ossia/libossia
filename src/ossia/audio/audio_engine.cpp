#include <ossia/audio/audio_engine.hpp>
#include <ossia/audio/dummy_protocol.hpp>
#include <ossia/audio/jack_protocol.hpp>
#include <ossia/audio/portaudio_protocol.hpp>
#include <ossia/audio/pulseaudio_protocol.hpp>
#include <ossia/audio/sdl_protocol.hpp>
#include <thread>

namespace ossia
{

namespace
{
struct default_audio_tick
{
  void operator()(const ossia::audio_tick_state& t) const noexcept
  {
    if(t.n_out > 0)
    {
      for(int i = 0; i < t.n_out; i++)
        for(std::size_t k = 0; k < t.frames; k++)
          t.outputs[i][k] = 0.f;
    }
  }
};
}

audio_engine::audio_engine()
{
  audio_tick = default_audio_tick{};
}

audio_engine::~audio_engine()
{
}

void audio_engine::stop()
{
  int timeout = 5000;
  req_stop++;
  stop_processing = true;
  while (ack_stop != req_stop && running() && timeout > 0)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timeout -= 100;
  }

  std::atomic_thread_fence(std::memory_order_seq_cst);

  if(timeout <= 0)
    throw std::runtime_error("Audio thread not responding");
}

void audio_engine::start()
{
  stop_processing = false;
  std::atomic_thread_fence(std::memory_order_seq_cst);
}

void audio_engine::gc()
{
  // try to make deallocations happen in the main thread as far as possible
  fun_type t;
  while(tick_gc.try_dequeue(t));
}

void audio_engine::set_tick(audio_engine::fun_type&& t)
{
  int timeout = 5000;

  if(t.allocated())
    tick_funlist.enqueue(std::move(t));
  else
    tick_funlist.enqueue(default_audio_tick{});

  req_tick++;

  while (ack_tick != req_tick && running() && timeout > 0)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timeout -= 100;
  }

  std::atomic_thread_fence(std::memory_order_seq_cst);

  if(timeout <= 0)
    throw std::runtime_error("Audio thread not responding");
}

void audio_engine::load_audio_tick()
{
  fun_type tick;
  while(tick_funlist.try_dequeue(tick))
  {
    tick_gc.enqueue(std::move(audio_tick));
    audio_tick = std::move(tick);
    ack_tick++;
  }
}


ossia::audio_engine* make_audio_engine(
    std::string proto, std::string name, std::string req_in,
    std::string req_out, int& inputs, int& outputs, int& rate, int& bs)
{
  ossia::audio_engine* p{};

#if defined(__EMSCRIPTEN__)
  rate = 44100;
  bs = 8192;
  inputs = 0;
  outputs = 2;
  return new ossia::sdl_protocol{rate, bs};
#endif

  if (0)
  {
  }
#if __has_include(<pulse/pulseaudio.h>)
  else if (proto == "PulseAudio")
  {
    p = new ossia::pulseaudio_engine{name, req_in, req_out, inputs, outputs, rate,   bs};
  }
#endif
#if __has_include(<portaudio.h>)
  else if (proto == "PortAudio")
  {
    p = new ossia::portaudio_engine{name,    req_in, req_out, inputs,
                                    outputs, rate,   bs, paInDevelopment};
  }
#endif

#if __has_include(<jack/jack.h>) && defined(USE_WEAK_JACK)
  else if (proto == "JACK")
  {
    p = new ossia::jack_engine{std::make_shared<jack_client>(name), inputs, outputs};
  }
#endif

#if defined(OSSIA_AUDIO_SDL)
  else if (proto == "SDL")
  {
    inputs = 0;
    outputs = 2;
    return new ossia::sdl_protocol{rate, bs};
  }
#endif

  else if (proto == "Dummy")
  {
    p = new ossia::dummy_engine{rate, bs};
  }

  if (!p)
  {
#if __has_include(<pulse/pulseaudio.h>)
    p = new ossia::pulseaudio_engine{name, req_in, req_out, inputs, outputs, rate,   bs};
#endif
  }

  if (!p)
  {
#if __has_include(<portaudio.h>)
    p = new ossia::portaudio_engine{name,    req_in, req_out, inputs,
                                    outputs, rate,   bs, paInDevelopment};
#endif
  }

  if (!p)
  {
#if __has_include(<jack/jack.h>) && defined(USE_WEAK_JACK)
    p = new ossia::jack_engine{std::make_shared<jack_client>(name), inputs, outputs};
#endif
  }

  if (!p)
  {
#if defined(OSSIA_AUDIO_SDL)
    inputs = 0;
    outputs = 2;
    p = new ossia::sdl_protocol{rate, bs};
#endif
  }

  if (!p)
  {
    p = new ossia::dummy_engine{rate, bs};
  }

  return p;
}

}
