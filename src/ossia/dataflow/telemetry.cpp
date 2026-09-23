#include <ossia/dataflow/telemetry.hpp>

#include <algorithm>
#include <cmath>

namespace ossia::telemetry
{
void meter_levels::clear() noexcept
{
  const std::size_t n = std::min<std::size_t>(channels, max_meter_channels);
  std::fill_n(peak.begin(), n, 0.f);
  std::fill_n(sum_squares.begin(), n, 0.f);
  clipped.fill(0);
  channels = 0;
  ticks = 0;
  frames = 0;
}

void meter_levels::merge(const meter_levels& other) noexcept
{
  const std::size_t n = std::min<std::size_t>(
      std::max(channels, other.channels), max_meter_channels);
  for(std::size_t c = channels; c < n; c++)
  {
    peak[c] = 0.f;
    sum_squares[c] = 0.f;
  }
  for(std::size_t c = 0; c < std::min<std::size_t>(other.channels, n); c++)
  {
    peak[c] = std::max(peak[c], other.peak[c]);
    sum_squares[c] += other.sum_squares[c];
  }
  for(std::size_t i = 0; i < clipped.size(); i++)
    clipped[i] |= other.clipped[i];
  channels = n;
  ticks += other.ticks;
  frames += other.frames;
}

namespace
{
template <typename Sample>
void accumulate_channel(
    meter_levels& m, std::size_t c, const Sample* samples, std::size_t n) noexcept
{
  float p = m.peak[c];
  float s = 0.f;
  bool clip = false;
  for(std::size_t i = 0; i < n; i++)
  {
    const float x = std::abs(float(samples[i]));
    p = std::max(p, x);
    s += x * x;
    clip |= x >= 1.f;
  }
  m.peak[c] = p;
  m.sum_squares[c] += s;
  if(clip)
    m.clipped[c / 32] |= 1u << (c % 32);
}

// A channel that appears mid-way starts from silence.
void grow_to(meter_levels& m, std::size_t chans) noexcept
{
  chans = std::min(chans, max_meter_channels);
  for(std::size_t c = m.channels; c < chans; c++)
  {
    m.peak[c] = 0.f;
    m.sum_squares[c] = 0.f;
  }
  m.channels = std::max<uint32_t>(m.channels, chans);
}
}

void meter_levels::accumulate(const ossia::audio_vector& chans) noexcept
{
  grow_to(*this, chans.size());
  std::size_t n = 0;
  for(std::size_t c = 0; c < channels && c < chans.size(); c++)
  {
    accumulate_channel(*this, c, chans[c].data(), chans[c].size());
    n = std::max(n, chans[c].size());
  }
  ticks++;
  frames += n;
}

void meter_levels::accumulate(
    const float* const* chans, int count, std::size_t n) noexcept
{
  grow_to(*this, std::max(count, 0));
  for(std::size_t c = 0; c < channels && c < std::size_t(count); c++)
    accumulate_channel(*this, c, chans[c], n);
  ticks++;
  frames += n;
}

float meter_levels::rms(std::size_t chan) const noexcept
{
  if(chan >= channels || frames == 0)
    return 0.f;
  return std::sqrt(sum_squares[chan] / float(frames));
}

namespace
{
frame make_frame(std::size_t capacity)
{
  frame f;
  f.meters.resize(capacity);
  return f;
}
}

arena::arena(std::size_t meter_capacity)
    : m_sources(meter_capacity)
    , m_unread(meter_capacity)
    , m_staging{make_frame(meter_capacity)}
    , m_buffer{make_frame(meter_capacity)}
    , m_read{make_frame(meter_capacity)}
{
}

arena::~arena() = default;

void arena::attach(
    std::size_t index, uint32_t generation, tap_kind kind,
    std::shared_ptr<meter_tap>& tap) noexcept
{
  if(index >= m_sources.size())
    return;

  auto& src = m_sources[index];
  std::swap(src.tap, tap);
  src.generation = generation;
  src.kind = src.tap ? kind : tap_kind::none;
  m_unread[index].generation = generation;
  m_unread[index].levels.clear();
}

void arena::accumulate_hardware(const ossia::audio_tick_state& st) noexcept
{
  for(auto& src : m_sources)
  {
    switch(src.kind)
    {
      case tap_kind::hardware_inputs:
        src.tap->pending.accumulate(st.inputs, st.n_in, st.frames);
        break;
      case tap_kind::hardware_outputs:
        src.tap->pending.accumulate(st.outputs, st.n_out, st.frames);
        break;
      default:
        break;
    }
  }
}

void arena::tick(std::size_t frames, int sample_rate) noexcept
{
  m_frames += frames;
  m_frames_since_publish += frames;
  m_sample_rate = sample_rate;
  if(m_frames_since_publish >= m_publish_interval.load(std::memory_order_relaxed))
    publish();
}

void arena::publish() noexcept
{
  m_frames_since_publish = 0;

  // A frame still waiting in the buffer has not been read: fold what came
  // since into it rather than replace it.
  const bool unread = m_buffer.has_new_data();

  for(std::size_t i = 0; i < m_sources.size(); i++)
  {
    auto& src = m_sources[i];
    auto& acc = m_unread[i].levels;
    if(!unread)
      acc.clear();
    if(src.tap)
    {
      acc.merge(src.tap->pending);
      src.tap->pending.clear();
    }
    m_staging.meters[i].generation = m_unread[i].generation;
    m_staging.meters[i].levels = acc;
  }

  m_staging.publish_seq = ++m_seq;
  m_staging.frames = m_frames;
  m_staging.sample_rate = m_sample_rate;
  m_buffer.produce(m_staging);
}
}
