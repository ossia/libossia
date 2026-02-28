#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_ASIO)
#include <ossia/audio/audio_engine.hpp>
#include <ossia/detail/thread.hpp>

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
#define NOMINMAX
#endif


#include <asiodrivers.h>
#include <asio.h>
#include <iasiodrv.h>

bool loadAsioDriver(char *name);
extern AsioDrivers* asioDrivers;

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#define OSSIA_AUDIO_ASIO 1

namespace ossia
{
struct asio_card
{
  std::string name;
  int driver_index{-1};
};

// Must be a singleton — ASIO SDK only supports one loaded driver at a time.
// We store the active engine pointer in a global so the static ASIO callbacks
// can reach it.
class asio_engine;
namespace detail
{
inline asio_engine* current_asio_engine = nullptr;
}

class asio_engine final : public audio_engine
{
public:
  asio_engine(
      std::string driver_name, int inputs, int outputs, int rate, int bs)
  {
    if(detail::current_asio_engine)
      throw std::runtime_error("ASIO error: only one ASIO engine can be active at a time");

    detail::current_asio_engine = this;

    // Load the ASIO driver
    if(!loadAsioDriver(const_cast<char*>(driver_name.c_str())))
    {
      detail::current_asio_engine = nullptr;
      throw std::runtime_error("ASIO error: could not load driver '" + driver_name + "'");
    }

    // Initialize the driver
    ASIODriverInfo driverInfo{};
    driverInfo.asioVersion = 2;
    driverInfo.sysRef = nullptr; // no window handle needed for headless operation

    if(ASIOInit(&driverInfo) != ASE_OK)
    {
      unload_driver();
      throw std::runtime_error(
          std::string("ASIO error: ASIOInit failed: ") + driverInfo.errorMessage);
    }
    m_initialized = true;

    // Query driver capabilities
    long maxInputChannels = 0, maxOutputChannels = 0;
    if(ASIOGetChannels(&maxInputChannels, &maxOutputChannels) != ASE_OK)
    {
      cleanup();
      throw std::runtime_error("ASIO error: ASIOGetChannels failed");
    }

    inputs = std::min((long)inputs, maxInputChannels);
    outputs = std::min((long)outputs, maxOutputChannels);

    if(inputs == 0 && outputs == 0)
    {
      cleanup();
      throw std::runtime_error("ASIO error: no channels available");
    }

    // Set sample rate
    if(ASIOCanSampleRate((ASIOSampleRate)rate) != ASE_OK)
    {
      // Try to use the driver's current rate
      ASIOSampleRate currentRate = 0;
      if(ASIOGetSampleRate(&currentRate) == ASE_OK && currentRate > 0)
      {
        rate = (int)currentRate;
      }
      else
      {
        cleanup();
        throw std::runtime_error("ASIO error: sample rate not supported");
      }
    }
    else
    {
      if(ASIOSetSampleRate((ASIOSampleRate)rate) != ASE_OK)
      {
        cleanup();
        throw std::runtime_error("ASIO error: could not set sample rate");
      }
    }

    // Query buffer size
    long minSize = 0, maxSize = 0, preferredSize = 0, granularity = 0;
    if(ASIOGetBufferSize(&minSize, &maxSize, &preferredSize, &granularity) != ASE_OK)
    {
      cleanup();
      throw std::runtime_error("ASIO error: ASIOGetBufferSize failed");
    }

    // Clamp requested buffer size to driver constraints
    if(bs <= 0)
      bs = preferredSize;
    bs = std::max(bs, (int)minSize);
    bs = std::min(bs, (int)maxSize);

    // For power-of-2 granularity, snap to nearest valid power of 2
    if(granularity == -1)
    {
      int pot = minSize;
      while(pot < bs && pot < maxSize)
        pot *= 2;
      bs = pot;
    }
    else if(granularity > 0)
    {
      bs = ((bs + granularity - 1) / granularity) * granularity;
      bs = std::min(bs, (int)maxSize);
    }

    m_bufferSize = bs;
    m_inputCount = inputs;
    m_outputCount = outputs;

    // Allocate buffer info structures
    int totalChannels = inputs + outputs;
    m_bufferInfos.resize(totalChannels);
    m_channelInfos.resize(totalChannels);

    int idx = 0;
    for(int i = 0; i < inputs; i++, idx++)
    {
      m_bufferInfos[idx].isInput = ASIOTrue;
      m_bufferInfos[idx].channelNum = i;
      m_bufferInfos[idx].buffers[0] = m_bufferInfos[idx].buffers[1] = nullptr;
    }
    for(int i = 0; i < outputs; i++, idx++)
    {
      m_bufferInfos[idx].isInput = ASIOFalse;
      m_bufferInfos[idx].channelNum = i;
      m_bufferInfos[idx].buffers[0] = m_bufferInfos[idx].buffers[1] = nullptr;
    }

    // Set up callbacks
    m_asioCallbacks.bufferSwitch = &asio_bufferSwitch;
    m_asioCallbacks.sampleRateDidChange = &asio_sampleRateDidChange;
    m_asioCallbacks.asioMessage = &asio_message;
    m_asioCallbacks.bufferSwitchTimeInfo = &asio_bufferSwitchTimeInfo;

    // Create buffers
    if(ASIOCreateBuffers(
           m_bufferInfos.data(), totalChannels, m_bufferSize, &m_asioCallbacks)
       != ASE_OK)
    {
      cleanup();
      throw std::runtime_error("ASIO error: ASIOCreateBuffers failed");
    }
    m_buffersCreated = true;

    // Get channel info (for sample format)
    for(int i = 0; i < totalChannels; i++)
    {
      m_channelInfos[i].channel = m_bufferInfos[i].channelNum;
      m_channelInfos[i].isInput = m_bufferInfos[i].isInput;
      if(ASIOGetChannelInfo(&m_channelInfos[i]) != ASE_OK)
      {
        cleanup();
        throw std::runtime_error("ASIO error: ASIOGetChannelInfo failed");
      }
    }

    // Check if ASIOOutputReady is supported
    m_postOutput = (ASIOOutputReady() == ASE_OK);

    // Allocate float conversion buffers (non-interleaved, one per channel)
    m_floatInputs.resize(inputs);
    m_floatOutputs.resize(outputs);
    m_inputPtrs.resize(inputs);
    m_outputPtrs.resize(outputs);
    for(int i = 0; i < inputs; i++)
    {
      m_floatInputs[i].resize(m_bufferSize);
      m_inputPtrs[i] = m_floatInputs[i].data();
    }
    for(int i = 0; i < outputs; i++)
    {
      m_floatOutputs[i].resize(m_bufferSize);
      m_outputPtrs[i] = m_floatOutputs[i].data();
    }

    // Set effective parameters
    this->effective_sample_rate = rate;
    this->effective_buffer_size = m_bufferSize;
    this->effective_inputs = inputs;
    this->effective_outputs = outputs;

    // Start processing
    if(ASIOStart() != ASE_OK)
    {
      cleanup();
      throw std::runtime_error("ASIO error: ASIOStart failed");
    }
    m_started = true;
  }

  bool running() const override { return m_started && !stop_processing; }

  void stop() override
  {
    audio_engine::stop();

    if(m_started)
    {
      ASIOStop();
      m_started = false;
    }
  }

  ~asio_engine() override
  {
    stop();
    cleanup();
  }

  static std::vector<asio_card> enumerate_drivers()
  {
    std::vector<asio_card> cards;

    AsioDrivers drivers;
    long numDrivers = drivers.asioGetNumDev();
    for(long i = 0; i < numDrivers; i++)
    {
      char name[128]{};
      if(drivers.asioGetDriverName(i, name, sizeof(name)) == 0)
      {
        cards.push_back({name, (int)i});
      }
    }
    return cards;
  }

  // Open the ASIO control panel for a given driver.
  // If the engine is currently running with that driver, calls ASIOControlPanel() directly.
  // Otherwise, temporarily loads the driver to show the panel.
  static void open_control_panel(const std::string& driver_name)
  {
    // If the engine is active and running this driver, just call the API directly
    if(detail::current_asio_engine)
    {
      ASIOControlPanel();
      return;
    }

    // No engine running — load driver temporarily
    if(!loadAsioDriver(const_cast<char*>(driver_name.c_str())))
      return;

    ASIODriverInfo info{};
    info.asioVersion = 2;
    if(ASIOInit(&info) == ASE_OK)
    {
      ASIOControlPanel();
      // Note: we do NOT call ASIOExit() here. Most ASIO control panels are
      // modal dialogs that block until dismissed, but some are not. Either way
      // the driver stays loaded while the panel is up. The next rescan or
      // engine creation will removeCurrentDriver() and reload as needed.
    }
  }

private:
  void cleanup()
  {
    if(m_started)
    {
      ASIOStop();
      m_started = false;
    }

    if(m_buffersCreated)
    {
      ASIODisposeBuffers();
      m_buffersCreated = false;
    }

    if(m_initialized)
    {
      ASIOExit();
      m_initialized = false;
    }

    unload_driver();
  }

  void unload_driver()
  {
    if(asioDrivers)
    {
      asioDrivers->removeCurrentDriver();
    }
    detail::current_asio_engine = nullptr;
  }

  // Convert ASIO native buffer to float for one channel
  static void convertToFloat(
      void* src, float* dst, long frames, ASIOSampleType type)
  {
    switch(type)
    {
      case ASIOSTFloat32LSB:
      {
        std::memcpy(dst, src, frames * sizeof(float));
        break;
      }
      case ASIOSTFloat64LSB:
      {
        auto* s = static_cast<double*>(src);
        for(long i = 0; i < frames; i++)
          dst[i] = (float)s[i];
        break;
      }
      case ASIOSTInt32LSB:
      {
        auto* s = static_cast<int32_t*>(src);
        constexpr float scale = 1.0f / 2147483648.0f;
        for(long i = 0; i < frames; i++)
          dst[i] = s[i] * scale;
        break;
      }
      case ASIOSTInt24LSB:
      {
        auto* s = static_cast<uint8_t*>(src);
        constexpr float scale = 1.0f / 8388608.0f;
        for(long i = 0; i < frames; i++)
        {
          int32_t val = (int32_t(s[i * 3 + 2]) << 24) | (int32_t(s[i * 3 + 1]) << 16)
                        | (int32_t(s[i * 3]) << 8);
          dst[i] = (val >> 8) * scale;
        }
        break;
      }
      case ASIOSTInt16LSB:
      {
        auto* s = static_cast<int16_t*>(src);
        constexpr float scale = 1.0f / 32768.0f;
        for(long i = 0; i < frames; i++)
          dst[i] = s[i] * scale;
        break;
      }
      case ASIOSTInt32LSB16:
      {
        auto* s = static_cast<int32_t*>(src);
        constexpr float scale = 1.0f / 32768.0f;
        for(long i = 0; i < frames; i++)
          dst[i] = (s[i] & 0xFFFF) * scale;
        break;
      }
      case ASIOSTInt32LSB18:
      {
        auto* s = static_cast<int32_t*>(src);
        constexpr float scale = 1.0f / 131072.0f;
        for(long i = 0; i < frames; i++)
          dst[i] = (s[i] & 0x3FFFF) * scale;
        break;
      }
      case ASIOSTInt32LSB20:
      {
        auto* s = static_cast<int32_t*>(src);
        constexpr float scale = 1.0f / 524288.0f;
        for(long i = 0; i < frames; i++)
          dst[i] = (s[i] & 0xFFFFF) * scale;
        break;
      }
      case ASIOSTInt32LSB24:
      {
        auto* s = static_cast<int32_t*>(src);
        constexpr float scale = 1.0f / 8388608.0f;
        for(long i = 0; i < frames; i++)
          dst[i] = (s[i] & 0xFFFFFF) * scale;
        break;
      }
      // MSB formats (big-endian) — rare on x86 but must be handled
      case ASIOSTFloat32MSB:
      {
        auto* s = static_cast<uint8_t*>(src);
        for(long i = 0; i < frames; i++)
        {
          uint32_t val = (uint32_t(s[i * 4]) << 24) | (uint32_t(s[i * 4 + 1]) << 16)
                         | (uint32_t(s[i * 4 + 2]) << 8) | uint32_t(s[i * 4 + 3]);
          float f;
          std::memcpy(&f, &val, 4);
          dst[i] = f;
        }
        break;
      }
      case ASIOSTInt32MSB:
      {
        auto* s = static_cast<uint8_t*>(src);
        constexpr float scale = 1.0f / 2147483648.0f;
        for(long i = 0; i < frames; i++)
        {
          int32_t val = (int32_t(s[i * 4]) << 24) | (int32_t(s[i * 4 + 1]) << 16)
                        | (int32_t(s[i * 4 + 2]) << 8) | int32_t(s[i * 4 + 3]);
          dst[i] = val * scale;
        }
        break;
      }
      case ASIOSTInt16MSB:
      {
        auto* s = static_cast<uint8_t*>(src);
        constexpr float scale = 1.0f / 32768.0f;
        for(long i = 0; i < frames; i++)
        {
          int16_t val = (int16_t(s[i * 2]) << 8) | int16_t(s[i * 2 + 1]);
          dst[i] = val * scale;
        }
        break;
      }
      default:
        std::memset(dst, 0, frames * sizeof(float));
        break;
    }
  }

  // Convert float to ASIO native buffer for one channel
  static void convertFromFloat(
      const float* src, void* dst, long frames, ASIOSampleType type)
  {
    switch(type)
    {
      case ASIOSTFloat32LSB:
      {
        std::memcpy(dst, src, frames * sizeof(float));
        break;
      }
      case ASIOSTFloat64LSB:
      {
        auto* d = static_cast<double*>(dst);
        for(long i = 0; i < frames; i++)
          d[i] = (double)src[i];
        break;
      }
      case ASIOSTInt32LSB:
      {
        auto* d = static_cast<int32_t*>(dst);
        constexpr double scale = 2147483647.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          d[i] = (int32_t)(val * scale);
        }
        break;
      }
      case ASIOSTInt24LSB:
      {
        auto* d = static_cast<uint8_t*>(dst);
        constexpr double scale = 8388607.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          int32_t s = (int32_t)(val * scale);
          d[i * 3] = (uint8_t)(s & 0xFF);
          d[i * 3 + 1] = (uint8_t)((s >> 8) & 0xFF);
          d[i * 3 + 2] = (uint8_t)((s >> 16) & 0xFF);
        }
        break;
      }
      case ASIOSTInt16LSB:
      {
        auto* d = static_cast<int16_t*>(dst);
        constexpr double scale = 32767.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          d[i] = (int16_t)(val * scale);
        }
        break;
      }
      case ASIOSTInt32LSB16:
      {
        auto* d = static_cast<int32_t*>(dst);
        constexpr double scale = 32767.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          d[i] = (int32_t)(val * scale);
        }
        break;
      }
      case ASIOSTInt32LSB18:
      {
        auto* d = static_cast<int32_t*>(dst);
        constexpr double scale = 131071.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          d[i] = (int32_t)(val * scale);
        }
        break;
      }
      case ASIOSTInt32LSB20:
      {
        auto* d = static_cast<int32_t*>(dst);
        constexpr double scale = 524287.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          d[i] = (int32_t)(val * scale);
        }
        break;
      }
      case ASIOSTInt32LSB24:
      {
        auto* d = static_cast<int32_t*>(dst);
        constexpr double scale = 8388607.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          d[i] = (int32_t)(val * scale);
        }
        break;
      }
      case ASIOSTFloat32MSB:
      {
        auto* d = static_cast<uint8_t*>(dst);
        for(long i = 0; i < frames; i++)
        {
          uint32_t val;
          std::memcpy(&val, &src[i], 4);
          d[i * 4] = (uint8_t)(val >> 24);
          d[i * 4 + 1] = (uint8_t)(val >> 16);
          d[i * 4 + 2] = (uint8_t)(val >> 8);
          d[i * 4 + 3] = (uint8_t)(val);
        }
        break;
      }
      case ASIOSTInt32MSB:
      {
        auto* d = static_cast<uint8_t*>(dst);
        constexpr double scale = 2147483647.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          int32_t s = (int32_t)(val * scale);
          d[i * 4] = (uint8_t)(s >> 24);
          d[i * 4 + 1] = (uint8_t)(s >> 16);
          d[i * 4 + 2] = (uint8_t)(s >> 8);
          d[i * 4 + 3] = (uint8_t)(s);
        }
        break;
      }
      case ASIOSTInt16MSB:
      {
        auto* d = static_cast<uint8_t*>(dst);
        constexpr double scale = 32767.0;
        for(long i = 0; i < frames; i++)
        {
          double val = std::max(-1.0, std::min(1.0, (double)src[i]));
          int16_t s = (int16_t)(val * scale);
          d[i * 2] = (uint8_t)(s >> 8);
          d[i * 2 + 1] = (uint8_t)(s);
        }
        break;
      }
      default:
        break;
    }
  }

  // === ASIO Callbacks (static, use global engine pointer) ===

  static ASIOTime* asio_bufferSwitchTimeInfo(
      ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
  {
    auto* self = detail::current_asio_engine;
    if(!self)
      return nullptr;

    [[maybe_unused]] static const thread_local auto _ = [] {
      ossia::set_thread_name("ossia audio 0");
      ossia::set_thread_pinned(thread_type::Audio, 0);
      return 0;
    }();

    self->tick_start();

    if(self->stop_processing)
    {
      self->tick_clear();
      // Clear output buffers
      for(int i = 0; i < self->m_outputCount; i++)
      {
        auto& info = self->m_bufferInfos[self->m_inputCount + i];
        auto& chanInfo = self->m_channelInfos[self->m_inputCount + i];
        void* buf = info.buffers[doubleBufferIndex];
        if(buf)
        {
          std::memset(buf, 0, self->sampleSize(chanInfo.type) * self->m_bufferSize);
        }
      }
      if(self->m_postOutput)
        ASIOOutputReady();
      return nullptr;
    }

    const long frames = self->m_bufferSize;

    // Convert ASIO input buffers -> float
    for(int i = 0; i < self->m_inputCount; i++)
    {
      void* src = self->m_bufferInfos[i].buffers[doubleBufferIndex];
      convertToFloat(src, self->m_inputPtrs[i], frames, self->m_channelInfos[i].type);
    }

    // Clear float output buffers
    for(int i = 0; i < self->m_outputCount; i++)
    {
      std::memset(self->m_outputPtrs[i], 0, frames * sizeof(float));
    }

    // Compute time — ASIO has no transport, so we only provide wall-clock
    // seconds (like PortAudio). samplePosition is a monotonic counter since
    // ASIOStart() and must NOT be used as position_in_frames (which is a
    // transport position that resets on stop/play).
    double seconds = 0.0;
    if(params && (params->timeInfo.flags & kSystemTimeValid))
    {
      seconds = asioSamplesToDouble(params->timeInfo.systemTime) * 1e-9;
    }

    // Call audio tick
    ossia::audio_tick_state ts{
        const_cast<float* const*>(self->m_inputPtrs.data()),
        self->m_outputPtrs.data(),
        self->m_inputCount,
        self->m_outputCount,
        (uint64_t)frames,
        seconds};

    self->audio_tick(ts);
    self->tick_end();

    // Convert float output buffers -> ASIO native format
    for(int i = 0; i < self->m_outputCount; i++)
    {
      int chIdx = self->m_inputCount + i;
      void* dst = self->m_bufferInfos[chIdx].buffers[doubleBufferIndex];
      convertFromFloat(
          self->m_outputPtrs[i], dst, frames, self->m_channelInfos[chIdx].type);
    }

    if(self->m_postOutput)
      ASIOOutputReady();

    return nullptr;
  }

  static void asio_bufferSwitch(long doubleBufferIndex, ASIOBool directProcess)
  {
    // Construct time info and delegate to the time-info callback
    ASIOTime timeInfo{};
    std::memset(&timeInfo, 0, sizeof(ASIOTime));

    ASIOSamples sPos;
    ASIOTimeStamp tStamp;
    if(ASIOGetSamplePosition(&sPos, &tStamp) == ASE_OK)
    {
      timeInfo.timeInfo.samplePosition = sPos;
      timeInfo.timeInfo.systemTime = tStamp;
      timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
    }

    ASIOSampleRate sRate = 0;
    if(ASIOGetSampleRate(&sRate) == ASE_OK)
    {
      timeInfo.timeInfo.sampleRate = sRate;
      timeInfo.timeInfo.flags |= kSampleRateValid;
    }

    asio_bufferSwitchTimeInfo(&timeInfo, doubleBufferIndex, directProcess);
  }

  static void asio_sampleRateDidChange(ASIOSampleRate sRate)
  {
    auto* self = detail::current_asio_engine;
    if(self && sRate > 0)
    {
      self->effective_sample_rate = (int)sRate;
    }
  }

  static long asio_message(long selector, long value, void* message, double* opt)
  {
    switch(selector)
    {
      case kAsioSelectorSupported:
        switch(value)
        {
          case kAsioEngineVersion:
          case kAsioSupportsTimeInfo:
          case kAsioResetRequest:
          case kAsioResyncRequest:
          case kAsioLatenciesChanged:
          case kAsioOverload:
            return 1;
          default:
            return 0;
        }

      case kAsioEngineVersion:
        return 2;

      case kAsioSupportsTimeInfo:
        return 1;

      case kAsioResetRequest:
        // Driver requests reset — for now, just acknowledge
        return 1;

      case kAsioResyncRequest:
        return 1;

      case kAsioLatenciesChanged:
        return 1;

      case kAsioOverload:
        return 1;

      default:
        return 0;
    }
  }

  static double asioSamplesToDouble(const ASIOSamples& s)
  {
#if NATIVE_INT64
    return (double)s;
#else
    return s.hi * 4294967296.0 + s.lo;
#endif
  }

  static double asioSamplesToDouble(const ASIOTimeStamp& s)
  {
#if NATIVE_INT64
    return (double)s;
#else
    return s.hi * 4294967296.0 + s.lo;
#endif
  }

  static long sampleSize(ASIOSampleType type)
  {
    switch(type)
    {
      case ASIOSTInt16MSB:
      case ASIOSTInt16LSB:
        return 2;
      case ASIOSTInt24MSB:
      case ASIOSTInt24LSB:
        return 3;
      case ASIOSTInt32MSB:
      case ASIOSTInt32LSB:
      case ASIOSTFloat32MSB:
      case ASIOSTFloat32LSB:
      case ASIOSTInt32MSB16:
      case ASIOSTInt32MSB18:
      case ASIOSTInt32MSB20:
      case ASIOSTInt32MSB24:
      case ASIOSTInt32LSB16:
      case ASIOSTInt32LSB18:
      case ASIOSTInt32LSB20:
      case ASIOSTInt32LSB24:
        return 4;
      case ASIOSTFloat64MSB:
      case ASIOSTFloat64LSB:
        return 8;
      default:
        return 4;
    }
  }

  std::vector<ASIOBufferInfo> m_bufferInfos;
  std::vector<ASIOChannelInfo> m_channelInfos;
  ASIOCallbacks m_asioCallbacks{};

  // Float conversion buffers
  std::vector<std::vector<float>> m_floatInputs;
  std::vector<std::vector<float>> m_floatOutputs;
  std::vector<float*> m_inputPtrs;
  std::vector<float*> m_outputPtrs;

  int m_bufferSize{};
  int m_inputCount{};
  int m_outputCount{};
  bool m_postOutput{};
  bool m_initialized{};
  bool m_buffersCreated{};
  bool m_started{};
};
}

#endif
