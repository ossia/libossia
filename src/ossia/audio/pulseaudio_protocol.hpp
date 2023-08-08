#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_PULSEAUDIO)

#if __has_include(<pulse/pulseaudio.h>)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/dylib_loader.hpp>
#include <ossia/detail/thread.hpp>

#include <pulse/pulseaudio.h>

#define OSSIA_AUDIO_PULSEAUDIO 1
namespace ossia
{

class libpulse
{
public:
  decltype(&::pa_threaded_mainloop_new) pa_threaded_mainloop_new{};
  decltype(&::pa_threaded_mainloop_free) pa_threaded_mainloop_free{};
  decltype(&::pa_threaded_mainloop_get_api) pa_threaded_mainloop_get_api{};
  decltype(&::pa_context_new) pa_context_new{};
  decltype(&::pa_context_ref) pa_context_ref{};
  decltype(&::pa_context_unref) pa_context_unref{};
  decltype(&::pa_context_set_state_callback) pa_context_set_state_callback{};
  decltype(&::pa_threaded_mainloop_lock) pa_threaded_mainloop_lock{};
  decltype(&::pa_threaded_mainloop_unlock) pa_threaded_mainloop_unlock{};
  decltype(&::pa_threaded_mainloop_start) pa_threaded_mainloop_start{};
  decltype(&::pa_threaded_mainloop_stop) pa_threaded_mainloop_stop{};
  decltype(&::pa_context_connect) pa_context_connect{};
  decltype(&::pa_context_get_state) pa_context_get_state{};
  decltype(&::pa_threaded_mainloop_wait) pa_threaded_mainloop_wait{};
  decltype(&::pa_channel_map_init_stereo) pa_channel_map_init_stereo{};
  decltype(&::pa_stream_new) pa_stream_new{};
  decltype(&::pa_stream_new_with_proplist) pa_stream_new_with_proplist{};
  decltype(&::pa_stream_ref) pa_stream_ref{};
  decltype(&::pa_stream_unref) pa_stream_unref{};
  decltype(&::pa_stream_get_state) pa_stream_get_state{};
  decltype(&::pa_stream_get_time) pa_stream_get_time{};
  decltype(&::pa_stream_set_state_callback) pa_stream_set_state_callback{};
  decltype(&::pa_stream_set_write_callback) pa_stream_set_write_callback{};
  decltype(&::pa_stream_set_read_callback) pa_stream_set_read_callback{};
  decltype(&::pa_stream_set_overflow_callback) pa_stream_set_overflow_callback{};
  decltype(&::pa_stream_connect_playback) pa_stream_connect_playback{};
  decltype(&::pa_stream_cork) pa_stream_cork{};
  decltype(&::pa_stream_is_corked) pa_stream_is_corked{};
  decltype(&::pa_threaded_mainloop_signal) pa_threaded_mainloop_signal{};
  decltype(&::pa_stream_begin_write) pa_stream_begin_write{};
  decltype(&::pa_stream_write) pa_stream_write{};
  decltype(&::pa_stream_set_name) pa_stream_set_name{};

  static const libpulse& instance()
  {
    static const libpulse self;
    return self;
  }

private:
  dylib_loader library;

  libpulse()
      : library("libpulse.so.0")
  {
    // in terms of regex:
    // decltype\(&::([a-z_]+)\) [a-z_]+{};
    // \1 = library.symbol<decltype(&::\1)>("\1");

    pa_threaded_mainloop_new = library.symbol<decltype(&::pa_threaded_mainloop_new)>(
        "pa_threaded_mainloop_new");
    pa_threaded_mainloop_free = library.symbol<decltype(&::pa_threaded_mainloop_free)>(
        "pa_threaded_mainloop_free");
    pa_threaded_mainloop_get_api
        = library.symbol<decltype(&::pa_threaded_mainloop_get_api)>(
            "pa_threaded_mainloop_get_api");
    pa_threaded_mainloop_lock = library.symbol<decltype(&::pa_threaded_mainloop_lock)>(
        "pa_threaded_mainloop_lock");
    pa_threaded_mainloop_unlock
        = library.symbol<decltype(&::pa_threaded_mainloop_unlock)>(
            "pa_threaded_mainloop_unlock");
    pa_threaded_mainloop_start = library.symbol<decltype(&::pa_threaded_mainloop_start)>(
        "pa_threaded_mainloop_start");
    pa_threaded_mainloop_stop = library.symbol<decltype(&::pa_threaded_mainloop_stop)>(
        "pa_threaded_mainloop_stop");
    pa_threaded_mainloop_signal
        = library.symbol<decltype(&::pa_threaded_mainloop_signal)>(
            "pa_threaded_mainloop_signal");
    pa_context_new = library.symbol<decltype(&::pa_context_new)>("pa_context_new");
    pa_context_ref = library.symbol<decltype(&::pa_context_ref)>("pa_context_ref");
    pa_context_unref = library.symbol<decltype(&::pa_context_unref)>("pa_context_unref");
    pa_context_set_state_callback
        = library.symbol<decltype(&::pa_context_set_state_callback)>(
            "pa_context_set_state_callback");
    pa_context_connect
        = library.symbol<decltype(&::pa_context_connect)>("pa_context_connect");
    pa_context_get_state
        = library.symbol<decltype(&::pa_context_get_state)>("pa_context_get_state");
    pa_threaded_mainloop_wait = library.symbol<decltype(&::pa_threaded_mainloop_wait)>(
        "pa_threaded_mainloop_wait");
    pa_channel_map_init_stereo = library.symbol<decltype(&::pa_channel_map_init_stereo)>(
        "pa_channel_map_init_stereo");
    pa_stream_new = library.symbol<decltype(&::pa_stream_new)>("pa_stream_new");
    pa_stream_new_with_proplist
        = library.symbol<decltype(&::pa_stream_new_with_proplist)>(
            "pa_stream_new_with_proplist");
    pa_stream_ref = library.symbol<decltype(&::pa_stream_ref)>("pa_stream_ref");
    pa_stream_unref = library.symbol<decltype(&::pa_stream_unref)>("pa_stream_unref");
    pa_stream_get_state
        = library.symbol<decltype(&::pa_stream_get_state)>("pa_stream_get_state");
    pa_stream_get_time
        = library.symbol<decltype(&::pa_stream_get_time)>("pa_stream_get_time");
    pa_stream_set_state_callback
        = library.symbol<decltype(&::pa_stream_set_state_callback)>(
            "pa_stream_set_state_callback");
    pa_stream_set_write_callback
        = library.symbol<decltype(&::pa_stream_set_write_callback)>(
            "pa_stream_set_write_callback");
    pa_stream_set_read_callback
        = library.symbol<decltype(&::pa_stream_set_read_callback)>(
            "pa_stream_set_read_callback");
    pa_stream_set_overflow_callback
        = library.symbol<decltype(&::pa_stream_set_overflow_callback)>(
            "pa_stream_set_overflow_callback");
    pa_stream_connect_playback = library.symbol<decltype(&::pa_stream_connect_playback)>(
        "pa_stream_connect_playback");
    pa_stream_cork = library.symbol<decltype(&::pa_stream_cork)>("pa_stream_cork");
    pa_stream_is_corked
        = library.symbol<decltype(&::pa_stream_is_corked)>("pa_stream_is_corked");
    pa_stream_begin_write
        = library.symbol<decltype(&::pa_stream_begin_write)>("pa_stream_begin_write");
    pa_stream_write = library.symbol<decltype(&::pa_stream_write)>("pa_stream_write");
    pa_stream_set_name
        = library.symbol<decltype(&::pa_stream_set_name)>("pa_stream_set_name");

    // in terms of regex:
    // decltype\(&::([a-z_]+)\) [a-z_]+{};
    // assert(\1);
    assert(pa_threaded_mainloop_new);
    assert(pa_threaded_mainloop_free);
    assert(pa_threaded_mainloop_get_api);
    assert(pa_threaded_mainloop_lock);
    assert(pa_threaded_mainloop_unlock);
    assert(pa_threaded_mainloop_start);
    assert(pa_threaded_mainloop_stop);
    assert(pa_threaded_mainloop_signal);
    assert(pa_context_new);
    assert(pa_context_ref);
    assert(pa_context_unref);
    assert(pa_context_set_state_callback);
    assert(pa_context_connect);
    assert(pa_context_get_state);
    assert(pa_threaded_mainloop_wait);
    assert(pa_channel_map_init_stereo);
    assert(pa_stream_new);
    assert(pa_stream_new_with_proplist);
    assert(pa_stream_ref);
    assert(pa_stream_unref);
    assert(pa_stream_get_state);
    assert(pa_stream_get_time);
    assert(pa_stream_set_state_callback);
    assert(pa_stream_set_write_callback);
    assert(pa_stream_set_read_callback);
    assert(pa_stream_set_overflow_callback);
    assert(pa_stream_connect_playback);
    assert(pa_stream_cork);
    assert(pa_stream_is_corked);
    assert(pa_stream_begin_write);
    assert(pa_stream_write);
    assert(pa_stream_set_name);
  }
};

class pulseaudio_engine final : public audio_engine
{
public:
  struct mainloop_locker
  {
    pulseaudio_engine& engine;
    mainloop_locker(pulseaudio_engine& engine)
        : engine{engine}
    {
      if(engine.m_mainloop)
      {
        const auto& pa = libpulse::instance();
        pa.pa_threaded_mainloop_lock(engine.m_mainloop);
      }
    }

    ~mainloop_locker()
    {
      if(engine.m_mainloop)
      {
        const auto& pa = libpulse::instance();
        pa.pa_threaded_mainloop_unlock(engine.m_mainloop);
      }
    }
  };

  using sample_rate_t = double;
  using buffer_size_t = uint32_t;

  pulseaudio_engine(
      std::string name, std::string card_in, std::string card_out, int inputs,
      int outputs, int rate, int bs)
  {
    inputs = 0;
    outputs = 2;
    bs = 512;
    rate = 48000;
    effective_inputs = inputs;
    effective_outputs = outputs;
    m_frames = bs;

    const auto& pa = libpulse::instance();

    m_mainloop = pa.pa_threaded_mainloop_new();
    if(!m_mainloop)
    {
      // let's switch to Either or Outcome shall we :-)
      throw std::runtime_error("Cannot initialize pulseaudio main loop");
    }

    m_api = pa.pa_threaded_mainloop_get_api(m_mainloop);
    if(!m_api)
    {
      throw std::runtime_error("Cannot initialize pulseaudio mainloop API");
    }

    // both for pulseaudio, and for JACK, instead of requesting a device,
    // you declare your app as being a device which will be a piece of the
    // audio graph - so there should be an API to set its name in that graph.
    m_context = pa.pa_context_new(m_api, m_name.data());
    if(!m_context)
    {
      throw std::runtime_error("Cannot initialize pulseaudio context");
    }

    auto context_callback = [](pa_context*, void* mainloop) {
      auto& pa = libpulse::instance();
      pa.pa_threaded_mainloop_signal((pa_threaded_mainloop*)mainloop, 0);
    };
    pa.pa_context_set_state_callback(m_context, context_callback, m_mainloop);

    mainloop_locker lock{*this};

    if(auto err = pa.pa_threaded_mainloop_start(m_mainloop); err != 0)
    {
      throw std::runtime_error("Cannot start pulseaudio main loop");
    }
    if(auto err
       = pa.pa_context_connect(m_context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
       err != 0)
    {
      throw std::runtime_error("Cannot conntext the pulseaudio context");
    }

    // Wait until everything is ready - completely arbitrary timeout, maybe
    // this should be settable
    using namespace std;
    using namespace std::literals;
    static const constexpr auto default_timeout = 3s;
    {
      auto t0 = chrono::steady_clock::now();
      bool timeout = false;
      bool ready = false;
      while((timeout = (chrono::steady_clock::now() - t0 < default_timeout)) && !ready)
      {
        switch(pa.pa_context_get_state(m_context))
        {
          case PA_CONTEXT_CONNECTING:
          case PA_CONTEXT_AUTHORIZING:
          case PA_CONTEXT_SETTING_NAME:
            pa.pa_threaded_mainloop_wait(m_mainloop);
            continue;

          case PA_CONTEXT_READY:
            ready = true;
            break;

          case PA_CONTEXT_FAILED:
          case PA_CONTEXT_TERMINATED:
          default:
            throw std::runtime_error("Invalid context state");
        }
      }

      if(!ready)
      {
        throw std::runtime_error("Context creation timeout");
      }
    }

    /// Everything from the beginning of start() to this point should
    /// actually go in a context object which is not part of the device.
    /// (A bit like Pa_Initialize() / Pa_Terminate() for portaudio)

    pa_sample_spec sample_specifications;
    sample_specifications.format = PA_SAMPLE_FLOAT32LE;

    // for pulseaudio, samplerate is an uint
    sample_specifications.rate = rate;

    // in pulse and JACK we ask how many channels we want ; in addition to
    // Jack, pulse does intelligent resampling / upmixing / downmixing
    sample_specifications.channels = 2;

    pa_channel_map map;
    pa.pa_channel_map_init_stereo(&map);

    m_stream = pa.pa_stream_new(m_context, "ossia", &sample_specifications, &map);
    if(!m_stream)
    {
      throw std::runtime_error("Cannot initialize pulseaudio stream");
    }

    const auto stream_callback = [](pa_stream* s, void* mainloop) {
      auto& pa = libpulse::instance();
      pa.pa_threaded_mainloop_signal((pa_threaded_mainloop*)mainloop, 0);
    };

    pa.pa_stream_set_state_callback(m_stream, stream_callback, m_mainloop);
    pa.pa_stream_set_write_callback(m_stream, output_callback, this);

    // Don't set values here, instead the server will provide what it judges to
    // be the best.
    pa_buffer_attr buffer_attr;
    buffer_attr.maxlength = m_frames * sizeof(float) * inputs;
    buffer_attr.tlength = m_frames * sizeof(float) * outputs;
    buffer_attr.prebuf = (uint32_t)-1;
    buffer_attr.minreq = 0;

    const auto stream_flags = static_cast<pa_stream_flags_t>(
        PA_STREAM_START_CORKED | PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_NOT_MONOTONIC
        | PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_ADJUST_LATENCY);

    // Connect stream to the default audio output sink
    if(auto err = pa.pa_stream_connect_playback(
           m_stream, nullptr, &buffer_attr, stream_flags, nullptr, nullptr);
       err != 0)
    {
      throw std::runtime_error("Cannot start pulseaudio stream");
    }

    {
      auto t0 = chrono::steady_clock::now();
      bool timeout = false;
      bool ready = false;
      while((timeout = (chrono::steady_clock::now() - t0 < default_timeout)) && !ready)
      {
        switch(pa.pa_stream_get_state(m_stream))
        {
          case PA_STREAM_CREATING:
            pa.pa_threaded_mainloop_wait(m_mainloop);
            break;
          case PA_STREAM_READY:
            ready = true;
            break;
          default:
            throw std::runtime_error("Invalid stream state");
        }
      }

      if(!ready)
      {
        throw std::runtime_error("Stream creation timeout");
      }
    }

    // cork means pausing, cork = 0 means resuming
    pa.pa_stream_cork(m_stream, 0, success_cb, this);
  }

  bool running() const override
  {
    auto& pa = libpulse::instance();
    return m_stream && pa.pa_stream_is_corked(m_stream);
  }

  ~pulseaudio_engine() override
  {
    stop();

    auto& pa = libpulse::instance();
    {
      mainloop_locker lock{*this};
      pa.pa_stream_unref(m_stream);
      m_stream = nullptr;
      pa.pa_context_unref(m_context);
      m_context = nullptr;
    }

    pa.pa_threaded_mainloop_stop(m_mainloop);

    pa.pa_threaded_mainloop_free(m_mainloop);
    m_mainloop = nullptr;
    m_api = nullptr;
    m_stream = nullptr;
  }

private:
  static int clearBuffers(float** float_output, unsigned long nframes, int outs)
  {
    for(int i = 0; i < outs; i++)
    {
      auto chan = float_output[i];
      for(std::size_t j = 0; j < nframes; j++)
        chan[j] = 0.f;
    }

    return 0;
  }

  static void success_cb(pa_stream*, int, void*) { }
  static void output_callback(pa_stream* stream, size_t requested_bytes, void* userdata)
  {
    static const thread_local auto _ = [] {
      ossia::set_thread_name("ossia audio 0");
      return 0;
    }();

    auto& self = *static_cast<pulseaudio_engine*>(userdata);
    self.tick_start();

    auto& pa = libpulse::instance();
    size_t bytes_to_fill = requested_bytes;
    float* buffer = nullptr;

    auto clt = self.m_stream.load();
    if(self.stop_processing || !clt)
    {
      self.tick_clear();
      do
      {
        // Do nothing
        if(auto res = pa.pa_stream_begin_write(stream, (void**)&buffer, &bytes_to_fill);
           res != 0)
        {
          // we're in huge trouble
          return;
        }

        std::fill_n(buffer, bytes_to_fill / sizeof(float), 0);

        if(auto res = pa.pa_stream_write(
               stream, buffer, bytes_to_fill, nullptr, 0LL, PA_SEEK_RELATIVE);
           res != 0)
        {
          // we're in huge trouble
          return;
        }

        requested_bytes -= bytes_to_fill;
        bytes_to_fill = requested_bytes;
      } while(requested_bytes > 0);

      return;
    }

    {

      do
      {
        auto res = pa.pa_stream_begin_write(stream, (void**)&buffer, &bytes_to_fill);
        if(res != 0)
        {
          // we're in huge trouble
          std::cerr << "no pa_stream_begin_write\n";
          return;
        }

        if(buffer)
        {
          const auto size = bytes_to_fill / (sizeof(float) * self.effective_outputs);
          {
            auto float_input = nullptr;
            auto float_output = ((float*)buffer);

            float* float_outputs[2];
            float_outputs[0] = (float*)alloca(sizeof(float) * size);
            float_outputs[1] = (float*)alloca(sizeof(float) * size);

            pa_usec_t usec{};
            pa.pa_stream_get_time(stream, &usec);

            ossia::audio_tick_state ts{float_input,
                                       float_outputs,
                                       (int)self.effective_inputs,
                                       (int)self.effective_outputs,
                                       size,
                                       usec / 1e6};
            self.audio_tick(ts);

            int k = 0;
            for(std::size_t i = 0; i < size; i++)
            {
              float_output[k++] = float_outputs[0][i];
              float_output[k++] = float_outputs[1][i];
            }
          }
        }
        else
        {
          std::cerr << "no buffer\n";
        }

        if(auto res = pa.pa_stream_write(
               stream, buffer, bytes_to_fill, nullptr, 0LL, PA_SEEK_RELATIVE);
           res != 0)
        {
          // we're in huge trouble
          return;
        }

        requested_bytes -= bytes_to_fill;
        bytes_to_fill = requested_bytes;
      } while(requested_bytes > 0);
    }

    self.tick_end();
  }

  std::string m_name;
  buffer_size_t m_frames{};

  pa_threaded_mainloop* m_mainloop{};
  pa_mainloop_api* m_api{};
  pa_context* m_context{};
  std::atomic<pa_stream*> m_stream{};
};
}

#endif
#endif
