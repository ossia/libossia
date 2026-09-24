#include <ossia/audio/alsa_protocol.hpp>
#include <ossia/audio/asio_protocol.hpp>
#include <ossia/audio/audio_engine.hpp>
#include <ossia/audio/dummy_protocol.hpp>
#include <ossia/audio/jack_protocol.hpp>
#include <ossia/audio/miniaudio_protocol.hpp>
#include <ossia/audio/pipewire_protocol.hpp>
#include <ossia/audio/portaudio_protocol.hpp>
#include <ossia/audio/pulseaudio_protocol.hpp>
#include <ossia/audio/sdl_protocol.hpp>
#include <ossia/detail/logger.hpp>

#include <chrono>
#include <cmath>
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
  // audio_engine starts in the "started" state
  stop_processing = false;
  audio_tick = default_audio_tick{};
}

audio_engine::~audio_engine() = default;

void audio_engine::wait(int milliseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void audio_engine::stop()
{
  stop_processing = true;
  sync();
  gc();
}

void audio_engine::sync()
{
  int64_t req = request.load();
  req++;
  request.store(req);

#if defined(__EMSCRIPTEN__)
  // On WASM, the audio runs in a worklet thread. We cannot spin-wait
  // on the main thread as it blocks the browser event loop entirely.
  // The tick will pick up the new request on its next callback.
  return;
#endif

  // The engine has started running, we wait for a couple iterations
  // to leave some time for the ticks to be updated
  int k = 0;
  int buffer_length_in_ms = 8;
  if(this->effective_sample_rate > 0)
    buffer_length_in_ms
        = std::ceil(this->effective_buffer_size * 1000. / this->effective_sample_rate);

  if(running())
  {
    while(this->reply.load() < req)
    {
      if(k++ > 20)
      {
        ossia::logger().error("Audio engine seems stuck?");
        break;
      }
      this->wait(2 * buffer_length_in_ms);
    }
  }
}

void audio_engine::run_between_ticks(const std::function<void()>& f)
{
  if(!running())
  {
    f();
    return;
  }

  m_betweenDone.store(false, std::memory_order_relaxed);
  m_between.store(&f, std::memory_order_release);

  int buffer_ms = 8;
  if(effective_sample_rate > 0)
    buffer_ms = std::max(
        1, int(std::ceil(effective_buffer_size * 1000. / effective_sample_rate)));

  // A few buffers' worth: a callback that does not come by then is not coming.
  const auto deadline
      = std::chrono::steady_clock::now() + std::chrono::milliseconds(8 * buffer_ms + 50);
  while(!m_betweenDone.load(std::memory_order_acquire))
  {
    if(std::chrono::steady_clock::now() > deadline)
    {
      // Taken back before the callback took it: it is safe to run it here.
      if(m_between.exchange(nullptr, std::memory_order_acq_rel) == &f)
      {
        ossia::logger().warn("Audio engine: no callback, running the change directly");
        f();
        return;
      }
      // The callback took it and is running it: it will be done shortly.
    }
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }
}

void audio_engine::gc()
{
  // try to make deallocations happen in the main thread as far as possible
  fun_type t;
  while(tick_gc.try_dequeue(t))
    ;
}

void audio_engine::set_tick(audio_engine::fun_type&& t)
{
  if(t.allocated())
    tick_funlist.enqueue(std::move(t));
  else
    tick_funlist.enqueue(default_audio_tick{});

  sync();
  gc();
}

void audio_engine::load_audio_tick()
{
  if(auto f = m_between.exchange(nullptr, std::memory_order_acq_rel))
  {
    (*f)();
    m_betweenDone.store(true, std::memory_order_release);
  }

  fun_type tick;
  while(tick_funlist.try_dequeue(tick))
  {
    tick_gc.enqueue(std::move(audio_tick));
    audio_tick = std::move(tick);
  }
  reply.store(request.load());
}

ossia::audio_engine* make_audio_engine(
    std::string proto, std::string name, std::string req_in, std::string req_out,
    int& inputs, int& outputs, int& rate, int& bs)
{
  proto = "JACK";
  ossia::audio_engine* p{};

#if defined(__EMSCRIPTEN__)
  rate = 48000;
  bs = 1024;
  inputs = 0;
  outputs = 2;
#if OSSIA_ENABLE_MINIAUDIO
  {
    static auto ctx = std::make_shared<ossia::miniaudio_context>();
    static bool ctx_init = false;
    if(!ctx_init)
    {
      auto cfg = ma_context_config_init();
      ma_context_init(nullptr, 0, &cfg, &ctx->context);
      ctx_init = true;
    }
    ma_device_id in_id{}, out_id{};
    return new ossia::miniaudio_engine{ctx, name, in_id, out_id, inputs, outputs, rate, bs};
  }
#else
  return new ossia::sdl_protocol{rate, bs};
#endif
#endif

  if(0)
  {
  }
#if OSSIA_AUDIO_PIPEWIRE
  else if(proto == "PipeWire")
  {
    ossia::audio_setup setup;
    setup.name = name;
    setup.card_in = "";
    setup.card_out = "";
    setup.rate = rate;
    setup.buffer_size = bs;
    for(int i = 0; i < inputs; i++)
      setup.inputs.push_back("in_" + std::to_string(i));
    for(int i = 0; i < outputs; i++)
      setup.outputs.push_back("out_" + std::to_string(i));

    if (auto client = libremidi::pipewire::shared_context())
      p = new ossia::pipewire_audio_protocol{std::move(client), setup};
  }
#endif

#if OSSIA_AUDIO_PULSEAUDIO
  else if(proto == "PulseAudio")
  {
    p = new ossia::pulseaudio_engine{name, req_in, req_out, inputs, outputs, rate, bs};
  }
#endif

#if OSSIA_AUDIO_PORTAUDIO
  else if(proto == "PortAudio")
  {
    p = new ossia::portaudio_engine{name,    req_in, req_out, inputs,
                                    outputs, rate,   bs,      paInDevelopment};
  }
#endif

#if OSSIA_AUDIO_JACK
  else if(proto == "JACK")
  {
    p = new ossia::jack_engine{std::make_shared<jack_client>(name), inputs, outputs};
  }
#endif

#if defined(OSSIA_AUDIO_SDL)
  else if(proto == "SDL")
  {
    inputs = 0;
    outputs = 2;
    return new ossia::sdl_protocol{rate, bs};
  }
#endif

  else if(proto == "Dummy")
  {
    p = new ossia::dummy_engine{rate, bs};
  }

  if(!p)
  {
#if OSSIA_AUDIO_PULSEAUDIO
    p = new ossia::pulseaudio_engine{name, req_in, req_out, inputs, outputs, rate, bs};
#endif
  }

  if(!p)
  {
#if OSSIA_AUDIO_PORTAUDIO
    p = new ossia::portaudio_engine{name,    req_in, req_out, inputs,
                                    outputs, rate,   bs,      paInDevelopment};
#endif
  }

  if(!p)
  {
#if OSSIA_AUDIO_JACK
    p = new ossia::jack_engine{std::make_shared<jack_client>(name), inputs, outputs};
#endif
  }

  if(!p)
  {
#if defined(OSSIA_AUDIO_SDL)
    inputs = 0;
    outputs = 2;
    p = new ossia::sdl_protocol{rate, bs};
#endif
  }

  if(!p)
  {
    p = new ossia::dummy_engine{rate, bs};
  }

  if(p)
  {
    inputs = p->effective_inputs;
    outputs = p->effective_outputs;
    rate = p->effective_sample_rate;
    bs = p->effective_buffer_size;
  }

  return p;
}

}
