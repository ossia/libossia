#pragma once
#include <ossia/detail/config.hpp>

#if __has_include(<miniaudio.h>)
#define OSSIA_ENABLE_MINIAUDIO 1
// The single-header implementation is compiled exactly once, in miniaudio.cpp.
// Every other includer of this header only sees declarations, so there is no
// duplicate-symbol clash when libossia and score-plugin-audio are statically
// linked into the same binary (e.g. wasm).
#if defined(__APPLE__)
#define MA_NO_RUNTIME_LINKING 1
#endif
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS 1
#if defined(__EMSCRIPTEN__)
#define MA_ENABLE_WEBAUDIO 1
#define MA_ENABLE_AUDIO_WORKLETS 1
// The AudioWorklet thread runs the whole ossia execution tick (every node,
// including deep gfx/ISF exec-node call chains). miniaudio allocates a fixed
// heap buffer for that thread's stack; its 128KB default is far too small and
// a deep tick silently overruns it, corrupting adjacent heap allocations
// (manifesting much later as OOB / null-function crashes on the main thread).
// Give it a desktop-sized stack instead.
#define MA_AUDIO_WORKLETS_THREAD_STACK_SIZE 4194304
#elif defined(_WIN32)
#define MA_ENABLE_WASAPI 1
#else
#define MA_ENABLE_COREAUDIO 1
#define MA_ENABLE_ALSA 1
#endif
#define MA_NO_WAV 1
#define MA_NO_FLAC 1
#define MA_NO_MP3 1
#define MA_NO_RESOURCE_MANAGER 1
#define MA_NO_NODE_GRAPH 1
#define MA_NO_GENERATION 1
#define MA_MAX_CHANNELS 1024
// The audio thread is created with this priority when the rtprio limit
// allows it; the callback raises it otherwise (it asks for the maximum by
// default, which the usual limit of 95 or 98 refuses).
#define MA_PTHREAD_REALTIME_THREAD_PRIORITY 95
// Export the miniaudio API from libossia so consumers (score-plugin-audio)
// link against this one copy instead of compiling their own. OSSIA_EXPORT is
// empty in static builds, dllexport/visibility when building the ossia shared
// lib, and dllimport when compiling against it.
#define MA_API OSSIA_EXPORT

#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/thread.hpp>

#include <miniaudio.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif
// The HAL (devices, their properties, aggregates) is macOS only: iOS has
// AVAudioSession instead.
#if defined(__APPLE__) && TARGET_OS_OSX && !defined(__EMSCRIPTEN__)
#define OSSIA_MINIAUDIO_COREAUDIO_HAL 1
#endif

#if defined(OSSIA_MINIAUDIO_COREAUDIO_HAL)
#include <Block.h>
#include <unistd.h>
#include <CoreAudio/CoreAudio.h>
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <string_view>
#include <thread>

#if defined(__EMSCRIPTEN__)
#include <emscripten/em_asm.h>
#endif

#define OSSIA_AUDIO_MINIAUDIO 1

namespace ossia
{
struct miniaudio_context
{
  ma_context context;
};

class miniaudio_engine final : public audio_engine
{

public:
  std::vector<const ma_device_info*> devices;
  bool is_duplex(const ma_device_id& card_in, const ma_device_id& card_out)
  {
#if defined(__APPLE__)
    return true;
    std::string_view i = card_in.coreaudio;
    std::string_view o = card_out.coreaudio;
    if(i.length() != o.length() || i.empty() || o.empty())
      return false;
    if(i.substr(0, i.size() - 1) == o.substr(0, o.size() - 1))
      return true;

#endif
    return memcmp(&card_in, &card_out, sizeof(ma_device_id)) == 0;
  }
  miniaudio_engine(
      std::shared_ptr<miniaudio_context> ctx, std::string name,
      const ma_device_id& card_in, const ma_device_id& card_out, int inputs, int outputs,
      int rate, int bs, ma_share_mode share = ma_share_mode_shared)
      : m_ctx{std::move(ctx)}
  {
    ma_device_type dtype = ma_device_type_duplex;
    if(inputs == 0)
      dtype = ma_device_type_playback;
    else if(outputs == 0)
      dtype = ma_device_type_capture;

    ma_device_config config = ma_device_config_init(dtype);

    config.sampleRate = rate;
    config.periodSizeInFrames = bs;
    // As for PortAudio: periods shorter than 2 ms get one more.
    config.periods = int64_t(bs) * 1000 < 2 * int64_t(rate) ? 4 : 3;
    // A zeroed device id means "system default device". Passing NULL as the
    // pDeviceID makes miniaudio open the OS default device and, on backends
    // that support it (WASAPI in particular), automatically follow the OS
    // default output/input when the user changes it while the stream runs.
    auto is_default_id = [](const ma_device_id& id) {
      ma_device_id zero{};
      return memcmp(&id, &zero, sizeof(ma_device_id)) == 0;
    };

    if(outputs > 0)
    {
      config.playback.pDeviceID = is_default_id(card_out) ? nullptr : &card_out;
#if defined(OSSIA_MINIAUDIO_COREAUDIO_HAL)
      // The device's own channel count: miniaudio's channel conversion, from
      // the count score asks for, mixes channels together (all those past the
      // 32nd into each other) and is far too slow at hundreds of channels.
      // score's channels are mapped one to one onto the device's in the
      // callback instead.
      config.playback.channels = 0;
#else
      // The other backends do not open the device's own count when asked for
      // none: ALSA and Web Audio open two channels, WASAPI its mix format
      // whatever is asked.
      config.playback.channels = outputs;
#endif
      config.playback.format = ma_format_f32;
    }

    if(inputs > 0)
    {
      config.capture.pDeviceID = is_default_id(card_in) ? nullptr : &card_in;
#if defined(OSSIA_MINIAUDIO_COREAUDIO_HAL)
      config.capture.channels = 0;
#else
      config.capture.channels = inputs;
#endif
      config.capture.format = ma_format_f32;
    }

    // An input and an output on two devices: one clock for both.
    join_devices(config);

    config.dataCallback = callback;
    config.notificationCallback = on_notification;

    config.performanceProfile = ma_performance_profile_low_latency;
    config.noFixedSizedCallback = false;
    config.noClip = false;
    config.noDisableDenormals = false;
    config.noPreSilencedOutputBuffer = false;

    config.pUserData = this;

    this->effective_buffer_size = bs;
    this->effective_sample_rate = rate;
    this->effective_inputs = inputs;
    this->effective_outputs = outputs;

    // An explicit device that the backend moves elsewhere (CoreAudio does
    // when it goes away) is a device lost: score must not play somewhere else.
    m_explicit_device = (outputs > 0 && config.playback.pDeviceID)
                        || (inputs > 0 && config.capture.pDeviceID);

    allow_rate_switch(config, rate);

    ma_result res = open_device(config, share);

    // Never resample: a device that runs at another rate than asked (it does
    // not support it, another program uses it) is reopened at its own rate,
    // which score then adopts.
    // Nor reblock: with CoreAudio the period is the device's buffer, clamped
    // to what it supports, and callbacks of another size come at uneven
    // intervals. Other backends' periods are the system mixer's, whatever is
    // asked: these keep the size asked for.
#if !defined(__EMSCRIPTEN__)
    // Not on WASM: the AudioWorklet cannot be created twice in a page.
    if(res == MA_SUCCESS)
    {
      const ma_uint32 device_rate = outputs > 0 ? m_stream.playback.internalSampleRate
                                                : m_stream.capture.internalSampleRate;
      const ma_uint32 device_period = outputs > 0
                                          ? m_stream.playback.internalPeriodSizeInFrames
                                          : m_stream.capture.internalPeriodSizeInFrames;
      bool reopen = false;
      if(device_rate > 0 && device_rate != config.sampleRate)
      {
        config.sampleRate = device_rate;
        reopen = true;
      }
#if defined(OSSIA_MINIAUDIO_COREAUDIO_HAL)
      if(device_period > 0 && device_period != config.periodSizeInFrames)
      {
        config.periodSizeInFrames = device_period;
        reopen = true;
      }
#else
      (void)device_period;
#endif
      if(reopen)
      {
        config.periods
            = int64_t(config.periodSizeInFrames) * 1000 < 2 * int64_t(config.sampleRate) ? 4 : 3;
        ma_device_uninit(&m_stream);
        res = open_device(config, share);
      }
    }
#endif
    if(res != MA_SUCCESS)
      throw std::runtime_error(
          std::string("miniaudio: cannot open the device: ") + ma_result_description(res));
    this->effective_sample_rate = config.sampleRate;
    this->effective_buffer_size = config.periodSizeInFrames;

    ins_data.resize(effective_inputs * effective_buffer_size + 16);
    outs_data.resize(effective_outputs * effective_buffer_size + 16);
    ins.resize(effective_inputs + 2);
    outs.resize(effective_outputs + 2);

    if(res = ma_device_start(&m_stream); res != MA_SUCCESS)
    {
      ma_device_uninit(&m_stream);
      throw std::runtime_error(
          std::string("miniaudio: cannot start the device: ")
          + ma_result_description(res));
    }

    m_device_outputs = outputs > 0 ? (int)m_stream.playback.channels : 0;
    m_device_inputs = inputs > 0 ? (int)m_stream.capture.channels : 0;

    watch_devices();

    auto to_client_frames = [rate = effective_sample_rate](ma_uint32 frames, ma_uint32 device_rate) {
      return device_rate > 0 ? int(int64_t(frames) * rate / device_rate) : int(frames);
    };
    if(outputs > 0)
      effective_output_latency = to_client_frames(
          m_stream.playback.internalPeriodSizeInFrames * m_stream.playback.internalPeriods,
          m_stream.playback.internalSampleRate);
    if(inputs > 0)
      effective_input_latency = to_client_frames(
          m_stream.capture.internalPeriodSizeInFrames,
          m_stream.capture.internalSampleRate);

#if defined(__EMSCRIPTEN__)
    // A duplex device only reaches the output from inside getUserMedia()'s
    // then(): a page with no microphone, or one whose capture fails for any
    // other reason, plays nothing at all. Connecting an already connected node
    // is a no-op, so doing it here costs nothing and makes playback independent
    // of capture.
    EM_ASM(
        {
          const node = emscriptenGetAudioObject($0);
          const ctx = emscriptenGetAudioObject($1);
          if(node && ctx)
            node.connect(ctx.destination);
        },
        m_stream.webaudio.audioWorklet, m_stream.webaudio.audioContext);
#endif

    m_active = true;
  }

  //! The device as miniaudio opened it: formats, channels, rates it negotiated.
  const ma_device& device() const noexcept { return m_stream; }

  //! Whether the device is used exclusively, without the system mixer.
  bool exclusive() const noexcept { return m_exclusive; }

  bool running() const override
  {
    return m_active && ma_device_get_state(&m_stream) == ma_device_state_started;
  }

  //! Once true, stays true: the callbacks run no tick any more, so that
  //! nothing plays on whatever device the backend moved the stream to.
  bool failed() const noexcept override
  {
    if(!m_active || stop_processing)
      return false;
    if(m_lost.load(std::memory_order_acquire))
      return true;

    bool lost = m_hal->dead.load(std::memory_order_acquire)
                || ma_device_get_state(&m_stream) == ma_device_state_stopped;

    // No callback for a long while: the backend stalled without telling.
    if(const auto last = m_last_callback.load(std::memory_order_acquire); last > 0)
    {
      const auto period = std::chrono::nanoseconds(
          int64_t(1e9 * effective_buffer_size / std::max(1, effective_sample_rate)));
      const auto limit = std::max<std::chrono::nanoseconds>(std::chrono::seconds(2), 8 * period);
      if(std::chrono::steady_clock::now().time_since_epoch().count() - last > limit.count())
        lost = true;
    }

    if(lost)
      m_lost.store(true, std::memory_order_release);
    return lost;
  }

  void stop() override
  {
    audio_engine::stop();

    if(m_active)
    {
      unwatch_devices();
      ma_device_stop(&m_stream);
      ma_device_uninit(&m_stream);
    }
    m_active = false;
    m_aggregate.reset();
  }

  ~miniaudio_engine() override { stop(); }

private:
  static void
  callback(ma_device* pDevice, void* output, const void* input, ma_uint32 nframes)
  {
#if !defined(__EMSCRIPTEN__)
    [[maybe_unused]]
    static const thread_local auto _
        = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
#if defined(__linux__)
      ossia::set_current_thread_realtime(95, true, false);
#endif
      return 0;
    }();
#endif

    auto& self = *static_cast<miniaudio_engine*>(pDevice->pUserData);
    self.tick_start();
#if !defined(__EMSCRIPTEN__)
    // Not on WASM: steady_clock is unavailable in an AudioWorklet, and a
    // browser suspends the audio of a page in the background, which is not
    // a device lost.
    self.m_last_callback.store(
        std::chrono::steady_clock::now().time_since_epoch().count(),
        std::memory_order_release);
#endif
    // Zero when the backend does not report it
    self.observed_sample_rate.store(
        self.m_hal->rate.load(std::memory_order_relaxed), std::memory_order_relaxed);

    // Output pre-silenced by miniaudio: nothing plays.
    if(self.stop_processing || self.m_lost.load(std::memory_order_acquire)
       || self.m_hal->dead.load(std::memory_order_acquire))
    {
      self.tick_clear();
      return;
    }

    auto ins = self.ins.data();
    auto ins_data = self.ins_data.data();
    for(int i = 0; i < self.effective_inputs; i++)
      ins[i] = ins_data + i * nframes;

    // score's input c is the device's input c; those past the device's last
    // one are silent.
    const float* in_samples = static_cast<const float*>(input);
    const int dev_in = self.m_device_inputs;
    const int mapped_in
        = std::max(0, std::min(self.effective_inputs, dev_in - self.m_input_offset));
    for(ma_uint32 f = 0; f < nframes; f++)
    {
      const float* frame = in_samples + f * dev_in + self.m_input_offset;
      for(int c = 0; c < mapped_in; c++)
        ins[c][f] = frame[c];
    }
    for(int c = mapped_in; c < self.effective_inputs; c++)
      std::fill_n(ins[c], nframes, 0.f);

    auto outs = self.outs.data();
    auto outs_data = self.outs_data.data();
    std::memset(outs_data, 0, sizeof(float) * self.effective_outputs * nframes);
    for(int i = 0; i < self.effective_outputs; i++)
      outs[i] = outs_data + i * nframes;

#if defined(__EMSCRIPTEN__)
    // On WASM audio worklets, std::chrono::steady_clock is unavailable.
    // Use the sample count to derive time instead.
    self.m_frames_elapsed += nframes;
    double nsecs = (double)self.m_frames_elapsed / self.effective_sample_rate;
#else
    if(!self.m_start)
      self.m_start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto nsecs
        = std::chrono::duration_cast<std::chrono::nanoseconds>(now - *self.m_start)
              .count()
          / 1e9;
#endif

    ossia::audio_tick_state ts{(float* const*)ins,     outs,    self.effective_inputs,
                               self.effective_outputs, nframes, nsecs};
    if(self.audio_tick)
      self.audio_tick(ts);

    self.tick_end();

    // score's output c is the device's output c; the device's other outputs
    // keep miniaudio's pre-silencing.
    float* out_samples = static_cast<float*>(output);
    const int dev_out = self.m_device_outputs;
    const int mapped_out = std::min(self.effective_outputs, dev_out);
    for(ma_uint32 f = 0; f < nframes; f++)
    {
      float* frame = out_samples + f * dev_out;
      for(int c = 0; c < mapped_out; c++)
        frame[c] = outs[c][f];
    }
  }

  ma_result open_device(ma_device_config& config, ma_share_mode share)
  {
    // Exclusive access (ALSA "hw:") gives a period of bs frames; shared access
    // goes through the system mixer (ALSA dmix), whose own period is used
    // whatever bs is. A device another program holds can only be shared.
    ma_result res = MA_ERROR;
    for(ma_share_mode mode : {share, ma_share_mode_shared})
    {
      config.playback.shareMode = mode;
      config.capture.shareMode = mode;
      // The performance profile only matters without a period size: no
      // retry with another one.
      res = ma_device_init(&m_ctx->context, &config, &m_stream);
      if(res == MA_SUCCESS)
      {
        m_exclusive = mode == ma_share_mode_exclusive;
        if(mode != share)
          ossia::logger().warn(
              "miniaudio: exclusive access refused, using the device in shared mode");
        break;
      }
      if(mode == ma_share_mode_shared)
        break;
    }
    return res;
  }

  static void on_notification(const ma_device_notification* n)
  {
    auto& self = *static_cast<miniaudio_engine*>(n->pDevice->pUserData);
    switch(n->type)
    {
      case ma_device_notification_type_stopped:
        // Also sent for the stops we ask for ourselves
        if(!self.stop_processing)
          self.m_lost.store(true, std::memory_order_release);
        break;
      case ma_device_notification_type_rerouted:
        if(self.m_explicit_device)
          self.m_lost.store(true, std::memory_order_release);
        break;
      default:
        break;
    }
  }

#if defined(OSSIA_MINIAUDIO_COREAUDIO_HAL)
  //! What the HAL listeners report; they may still run once removed, so they
  //! co-own this rather than reach into the engine.
  struct hal_state
  {
    std::atomic_bool dead{};
    std::atomic_int rate{};
  };

  static AudioObjectID hal_object(const ma_device_id* id, bool playback)
  {
    AudioObjectID dev{};
    UInt32 size = sizeof(dev);
    if(!id)
    {
      const AudioObjectPropertyAddress addr{
          playback ? kAudioHardwarePropertyDefaultOutputDevice
                   : kAudioHardwarePropertyDefaultInputDevice,
          kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
      AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, &dev);
      return dev;
    }
    CFStringRef uid
        = CFStringCreateWithCString(nullptr, id->coreaudio, kCFStringEncodingUTF8);
    if(!uid)
      return 0;
    const AudioObjectPropertyAddress addr{
        kAudioHardwarePropertyTranslateUIDToDevice, kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};
    AudioObjectGetPropertyData(
        kAudioObjectSystemObject, &addr, sizeof(uid), &uid, &size, &dev);
    CFRelease(uid);
    return dev;
  }

  static double hal_rate(AudioObjectID dev)
  {
    Float64 r{};
    UInt32 size = sizeof(r);
    const AudioObjectPropertyAddress addr{
        kAudioDevicePropertyNominalSampleRate, kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};
    AudioObjectGetPropertyData(dev, &addr, 0, nullptr, &size, &r);
    return r;
  }

  //! The device may be switched to `rate`: it supports it, and no other
  //! program is using it (their audio would change pitch or break).
  static bool hal_may_switch(AudioObjectID dev, int rate)
  {
    if(!dev)
      return false;
    if(hal_rate(dev) == rate)
      return true;

    UInt32 running{};
    UInt32 size = sizeof(running);
    const AudioObjectPropertyAddress somewhere{
        kAudioDevicePropertyDeviceIsRunningSomewhere, kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};
    if(AudioObjectGetPropertyData(dev, &somewhere, 0, nullptr, &size, &running) != noErr
       || running)
      return false;

    const AudioObjectPropertyAddress rates{
        kAudioDevicePropertyAvailableNominalSampleRates, kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};
    if(AudioObjectGetPropertyDataSize(dev, &rates, 0, nullptr, &size) != noErr)
      return false;
    std::vector<AudioValueRange> ranges(size / sizeof(AudioValueRange));
    if(AudioObjectGetPropertyData(dev, &rates, 0, nullptr, &size, ranges.data()) != noErr)
      return false;
    return std::any_of(ranges.begin(), ranges.end(), [rate](const AudioValueRange& r) {
      return r.mMinimum <= rate && rate <= r.mMaximum;
    });
  }

  //! Whether the system lists the device: miniaudio looks devices up there
  static bool hal_listed(AudioObjectID dev)
  {
    const AudioObjectPropertyAddress addr{
        kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain};
    UInt32 size{};
    if(AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr, 0, nullptr, &size)
       != noErr)
      return false;
    std::vector<AudioObjectID> ids(size / sizeof(AudioObjectID));
    if(AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, ids.data())
       != noErr)
      return false;
    ids.resize(size / sizeof(AudioObjectID));
    return std::find(ids.begin(), ids.end(), dev) != ids.end();
  }

  static int hal_channels(AudioObjectID dev, bool input)
  {
    const AudioObjectPropertyAddress addr{
        kAudioDevicePropertyStreamConfiguration,
        input ? kAudioObjectPropertyScopeInput : kAudioObjectPropertyScopeOutput,
        kAudioObjectPropertyElementMain};
    UInt32 size{};
    if(AudioObjectGetPropertyDataSize(dev, &addr, 0, nullptr, &size) != noErr || size == 0)
      return 0;
    std::vector<char> data(size);
    auto list = reinterpret_cast<AudioBufferList*>(data.data());
    if(AudioObjectGetPropertyData(dev, &addr, 0, nullptr, &size, list) != noErr)
      return 0;
    int n = 0;
    for(UInt32 i = 0; i < list->mNumberBuffers; i++)
      n += list->mBuffers[i].mNumberChannels;
    return n;
  }

  //! A private aggregate device: only this process sees it, and it goes with
  //! the engine (also when the engine fails to open it).
  struct aggregate_device
  {
    AudioObjectID id{};
    ma_device_id uid{};
    ~aggregate_device()
    {
      if(id)
        AudioHardwareDestroyAggregateDevice(id);
    }
  };

  // Each device has its own clock: separately, they drift apart, and
  // miniaudio's ring buffer between the two then under- or overruns - the
  // input skips or repeats. In an aggregate device the input device is
  // resampled to the output's clock (drift compensation), and miniaudio sees
  // a single device.
  void join_devices(ma_device_config& config)
  {
    if(m_ctx->context.backend != ma_backend_coreaudio
       || config.deviceType != ma_device_type_duplex || !config.playback.pDeviceID
       || !config.capture.pDeviceID
       || std::string_view{config.playback.pDeviceID->coreaudio}
              == std::string_view{config.capture.pDeviceID->coreaudio})
      return;

    const AudioObjectID out = hal_object(config.playback.pDeviceID, true);
    const AudioObjectID in = hal_object(config.capture.pDeviceID, false);
    if(!out || !in)
      return;

    static std::atomic_int counter{};
    const std::string uid = "io.ossia.score.duplex." + std::to_string(getpid()) + "."
                            + std::to_string(counter++);

    auto cfstr = [](std::string_view s) {
      return CFStringCreateWithBytes(
          nullptr, (const UInt8*)s.data(), s.size(), kCFStringEncodingUTF8, false);
    };
    auto desc = CFDictionaryCreateMutable(
        nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    auto name = cfstr("ossia score");
    auto cfuid = cfstr(uid);
    auto out_uid = cfstr(config.playback.pDeviceID->coreaudio);
    auto in_uid = cfstr(config.capture.pDeviceID->coreaudio);
    int one = 1;
    auto yes = CFNumberCreate(nullptr, kCFNumberIntType, &one);
    CFDictionarySetValue(desc, CFSTR(kAudioAggregateDeviceNameKey), name);
    CFDictionarySetValue(desc, CFSTR(kAudioAggregateDeviceUIDKey), cfuid);
    CFDictionarySetValue(desc, CFSTR(kAudioAggregateDeviceIsPrivateKey), yes);
    CFDictionarySetValue(desc, CFSTR(kAudioAggregateDeviceMainSubDeviceKey), out_uid);

    auto subs = CFArrayCreateMutable(nullptr, 0, &kCFTypeArrayCallBacks);
    auto out_sub = CFDictionaryCreateMutable(
        nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(out_sub, CFSTR(kAudioSubDeviceUIDKey), out_uid);
    auto in_sub = CFDictionaryCreateMutable(
        nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(in_sub, CFSTR(kAudioSubDeviceUIDKey), in_uid);
    CFDictionarySetValue(in_sub, CFSTR(kAudioSubDeviceDriftCompensationKey), yes);
    CFArrayAppendValue(subs, out_sub);
    CFArrayAppendValue(subs, in_sub);
    CFDictionarySetValue(desc, CFSTR(kAudioAggregateDeviceSubDeviceListKey), subs);

    auto agg = std::make_unique<aggregate_device>();
    const OSStatus st = AudioHardwareCreateAggregateDevice(desc, &agg->id);
    for(CFTypeRef r : {(CFTypeRef)in_sub, (CFTypeRef)out_sub, (CFTypeRef)subs,
                       (CFTypeRef)yes, (CFTypeRef)in_uid, (CFTypeRef)out_uid,
                       (CFTypeRef)cfuid, (CFTypeRef)name, (CFTypeRef)desc})
      CFRelease(r);
    if(st != noErr || !agg->id)
    {
      ossia::logger().warn(
          "miniaudio: could not join the input and output devices ({}): they will "
          "drift apart",
          (int)st);
      agg->id = 0;
      return;
    }

    std::copy_n(uid.data(), std::min(uid.size(), sizeof(agg->uid.coreaudio) - 1), agg->uid.coreaudio);
    // The HAL publishes the device and its streams asynchronously, and
    // miniaudio only finds it once it is listed with streams both ways
    const int inputs = hal_channels(out, true) + hal_channels(in, true);
    const int outputs = hal_channels(out, false) + hal_channels(in, false);
    auto ready = [&] {
      return hal_listed(agg->id) && hal_channels(agg->id, true) >= inputs
             && hal_channels(agg->id, false) >= outputs;
    };
    for(int i = 0; i < 200 && !ready(); i++)
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if(!ready())
    {
      ossia::logger().warn(
          "miniaudio: the device joining the input and output did not come up: they "
          "will drift apart");
      return;
    }

    m_input_offset = hal_channels(out, true);
    m_subdevices = {out, in};
    m_aggregate = std::move(agg);
    config.playback.pDeviceID = &m_aggregate->uid;
    config.capture.pDeviceID = &m_aggregate->uid;
  }
  std::unique_ptr<aggregate_device> m_aggregate;
  std::vector<AudioObjectID> m_subdevices;

  void allow_rate_switch(ma_device_config& config, int rate)
  {
    if(m_ctx->context.backend != ma_backend_coreaudio)
      return;
    bool ok = true;
    if(config.deviceType != ma_device_type_capture)
      ok &= hal_may_switch(hal_object(config.playback.pDeviceID, true), rate);
    if(config.deviceType != ma_device_type_playback)
      ok &= hal_may_switch(hal_object(config.capture.pDeviceID, false), rate);
    config.coreaudio.allowNominalSampleRateChange = ok ? MA_TRUE : MA_FALSE;
  }

  // The HAL tells when a device dies (a removed aggregate, an unplugged
  // card) and when its rate changes under the engine.
  void watch_devices()
  {
    if(m_ctx->context.backend != ma_backend_coreaudio)
      return;
    std::vector<AudioObjectID> devices{
        (AudioObjectID)m_stream.coreaudio.deviceObjectIDPlayback,
        (AudioObjectID)m_stream.coreaudio.deviceObjectIDCapture};
    // Joined devices: the aggregate lives on when one of them goes away
    devices.insert(devices.end(), m_subdevices.begin(), m_subdevices.end());
    for(AudioObjectID dev : devices)
    {
      if(dev == 0
         || std::find_if(
                m_watched.begin(), m_watched.end(),
                [dev](const auto& w) { return w.first == dev; })
                != m_watched.end())
        continue;
      if(m_watched.empty())
        m_hal->rate = (int)hal_rate(dev);

      auto hal = m_hal;
      // The rate reported is the first device's: the one the callbacks follow
      const bool main = m_watched.empty();
      auto block = Block_copy(^(UInt32 n, const AudioObjectPropertyAddress* addrs) {
        for(UInt32 i = 0; i < n; i++)
        {
          if(addrs[i].mSelector == kAudioDevicePropertyDeviceIsAlive)
            hal->dead.store(true, std::memory_order_release);
          else if(main && addrs[i].mSelector == kAudioDevicePropertyNominalSampleRate)
            hal->rate.store((int)hal_rate(dev), std::memory_order_release);
        }
      });
      for(auto sel : {kAudioDevicePropertyDeviceIsAlive, kAudioDevicePropertyNominalSampleRate})
      {
        const AudioObjectPropertyAddress addr{
            sel, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
        AudioObjectAddPropertyListenerBlock(dev, &addr, nullptr, block);
      }
      m_watched.emplace_back(dev, block);
    }
  }
  void unwatch_devices()
  {
    for(auto& [dev, block] : m_watched)
    {
      for(auto sel : {kAudioDevicePropertyDeviceIsAlive, kAudioDevicePropertyNominalSampleRate})
      {
        const AudioObjectPropertyAddress addr{
            sel, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
        AudioObjectRemovePropertyListenerBlock(dev, &addr, nullptr, block);
      }
      Block_release(block);
    }
    m_watched.clear();
  }
  std::vector<std::pair<AudioObjectID, AudioObjectPropertyListenerBlock>> m_watched;
  std::shared_ptr<hal_state> m_hal = std::make_shared<hal_state>();
#else
  struct hal_state
  {
    std::atomic_bool dead{};
    std::atomic_int rate{};
  };
  void allow_rate_switch(ma_device_config&, int) { }
  void join_devices(ma_device_config&) { }
  struct aggregate_device
  {
  };
  std::unique_ptr<aggregate_device> m_aggregate;
  void watch_devices() { }
  void unwatch_devices() { }
  std::shared_ptr<hal_state> m_hal = std::make_shared<hal_state>();
#endif

  std::shared_ptr<miniaudio_context> m_ctx;
  ma_device m_stream;
#if defined(__EMSCRIPTEN__)
  uint64_t m_frames_elapsed{};
#else
  std::optional<std::chrono::steady_clock::time_point> m_start;
#endif

  ossia::float_vector ins_data;
  ossia::pod_vector<float*> ins;
  ossia::float_vector outs_data;
  ossia::pod_vector<float*> outs;

  bool m_active{};
  bool m_exclusive{};
  bool m_explicit_device{};
  //! Channels the device was opened with, which score's map onto
  int m_device_inputs{};
  int m_device_outputs{};
  //! score's first input is the device's input at this index: with two
  //! devices joined, the output device's own inputs come first.
  int m_input_offset{};
  mutable std::atomic_bool m_lost{};
  //! steady_clock time of the last callback, 0 before the first one
  std::atomic<int64_t> m_last_callback{};
};
}

#endif
// #endif
