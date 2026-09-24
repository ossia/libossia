#include <ossia/dataflow/telemetry.hpp>

#include <algorithm>
#include <cmath>

namespace ossia::telemetry
{
void meter_levels::clear() noexcept
{
  const std::size_t n = std::min<std::size_t>(channels, max_meter_channels);
  std::fill_n(peak.begin(), n, 0.f);
  std::fill_n(sum_squares.begin(), n, 0.);
  std::fill_n(channel_frames.begin(), n, 0u);
  clipped.fill(0);
  channels = 0;
  ticks = 0;
  frames = 0;
}

namespace
{
// A channel that appears mid-way starts from silence.
void grow_to(meter_levels& m, std::size_t chans) noexcept
{
  chans = std::min(chans, max_meter_channels);
  for(std::size_t c = m.channels; c < chans; c++)
  {
    m.peak[c] = 0.f;
    m.sum_squares[c] = 0.;
    m.channel_frames[c] = 0;
  }
  m.channels = std::max<uint32_t>(m.channels, chans);
}

uint32_t saturating_add(uint32_t a, uint64_t b) noexcept
{
  const uint64_t r = uint64_t(a) + b;
  return r > 0xFFFFFFFFu ? 0xFFFFFFFFu : uint32_t(r);
}
}

void meter_levels::merge(const meter_levels& other) noexcept
{
  grow_to(*this, other.channels);
  for(std::size_t c = 0; c < std::min<std::size_t>(other.channels, max_meter_channels);
      c++)
  {
    peak[c] = std::max(peak[c], other.peak[c]);
    sum_squares[c] += other.sum_squares[c];
    channel_frames[c] = saturating_add(channel_frames[c], other.channel_frames[c]);
  }
  for(std::size_t i = 0; i < clipped.size(); i++)
    clipped[i] |= other.clipped[i];
  ticks += other.ticks;
  frames += other.frames;
}

void meter_levels::assign(const meter_levels& other) noexcept
{
  const std::size_t n = std::min<std::size_t>(other.channels, max_meter_channels);
  channels = other.channels;
  ticks = other.ticks;
  frames = other.frames;
  std::copy_n(other.peak.begin(), n, peak.begin());
  std::copy_n(other.sum_squares.begin(), n, sum_squares.begin());
  std::copy_n(other.channel_frames.begin(), n, channel_frames.begin());
  clipped = other.clipped;
}

namespace
{
template <typename Sample>
void accumulate_channel(
    meter_levels& m, std::size_t c, const Sample* samples, std::size_t n) noexcept
{
  float p = m.peak[c];
  double s = 0.;
  bool clip = false;
  for(std::size_t i = 0; i < n; i++)
  {
    const float x = std::abs(float(samples[i]));
    p = std::max(p, x);
    s += double(x) * x;
    clip |= x >= 1.f;
  }
  m.peak[c] = p;
  m.sum_squares[c] += s;
  m.channel_frames[c] = saturating_add(m.channel_frames[c], n);
  if(clip)
    m.clipped[c / 32] |= 1u << (c % 32);
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
  if(chan >= channels || chan >= max_meter_channels || channel_frames[chan] == 0)
    return 0.f;
  return float(std::sqrt(sum_squares[chan] / double(channel_frames[chan])));
}

namespace
{
frame make_frame(std::size_t meters, std::size_t benches, std::size_t playheads)
{
  frame f;
  f.meters.resize(meters);
  f.benches.resize(benches);
  f.playheads.resize(playheads);
  return f;
}
}

arena::arena(
    std::size_t meter_capacity, std::size_t bench_capacity,
    std::size_t playhead_capacity)
    : m_sources(meter_capacity)
    , m_unread(meter_capacity)
    , m_delta(meter_capacity)
    , m_benchSources(bench_capacity)
    , m_benchUnread(bench_capacity)
    , m_benchDelta(bench_capacity)
    , m_playheadSources(playhead_capacity)
    , m_buffer{make_frame(meter_capacity, bench_capacity, playhead_capacity)}
    , m_read{make_frame(meter_capacity, bench_capacity, playhead_capacity)}
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

void arena::attach_bench(
    std::size_t index, uint32_t generation, std::shared_ptr<bench_tap>& tap) noexcept
{
  if(index >= m_benchSources.size())
    return;

  auto& src = m_benchSources[index];
  std::swap(src.tap, tap);
  src.generation = generation;
  m_benchUnread[index].generation = generation;
  m_benchUnread[index].levels.clear();
}

void arena::attach_playhead(
    std::size_t index, uint32_t generation, std::shared_ptr<playhead_tap>& tap) noexcept
{
  if(index >= m_playheadSources.size())
    return;

  auto& src = m_playheadSources[index];
  std::swap(src.tap, tap);
  src.generation = generation;
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

void arena::stage(bool with_unread) noexcept
{
  auto& f = m_buffer.write_buffer();
  for(std::size_t i = 0; i < m_benchSources.size(); i++)
  {
    auto& slot = f.benches[i];
    slot.generation = m_benchUnread[i].generation;
    slot.levels = with_unread ? m_benchUnread[i].levels : bench_levels{};
    slot.levels.merge(m_benchDelta[i]);
  }

  for(std::size_t i = 0; i < m_sources.size(); i++)
  {
    auto& slot = f.meters[i];
    slot.generation = m_unread[i].generation;
    if(with_unread)
    {
      slot.levels.assign(m_unread[i].levels);
      slot.levels.merge(m_delta[i]);
    }
    else
    {
      slot.levels.assign(m_delta[i]);
    }
  }

  for(std::size_t i = 0; i < m_playheadSources.size(); i++)
  {
    const auto& src = m_playheadSources[i];
    auto& slot = f.playheads[i];
    slot.generation = src.generation;
    slot.date = src.tap ? src.tap->date : 0;
    slot.running = src.tap && src.tap->running;
  }

  f.publish_seq = ++m_seq;
  f.frames = m_frames;
  f.window_frames = (with_unread ? m_unreadFrames : 0) + m_frames_since_publish;
  f.sample_rate = m_sample_rate;
}

void arena::publish() noexcept
{
  // What came since the previous publication.
  for(std::size_t i = 0; i < m_benchSources.size(); i++)
  {
    auto& d = m_benchDelta[i];
    d.clear();
    if(auto& tap = m_benchSources[i].tap)
    {
      d = tap->pending;
      tap->pending.clear();
    }
  }
  for(std::size_t i = 0; i < m_sources.size(); i++)
  {
    auto& d = m_delta[i];
    d.clear();
    if(auto& tap = m_sources[i].tap)
    {
      d.assign(tap->pending);
      tap->pending.clear();
    }
  }

  // A frame still waiting has not been read: this one replaces it, so it
  // carries what it had too. Whether it was read is only known for sure when
  // replacing it; if the interface took it meanwhile, only the new part goes.
  bool folded = false;
  if(m_buffer.has_new_data())
  {
    stage(true);
    folded = m_buffer.publish_if_unread();
  }
  if(!folded)
  {
    stage(false);
    m_buffer.publish();
  }

  // What the interface has not read yet, as of this frame.
  for(std::size_t i = 0; i < m_sources.size(); i++)
  {
    auto& acc = m_unread[i].levels;
    if(folded)
      acc.merge(m_delta[i]);
    else
      acc.assign(m_delta[i]);
  }
  for(std::size_t i = 0; i < m_benchSources.size(); i++)
  {
    auto& acc = m_benchUnread[i].levels;
    if(!folded)
      acc.clear();
    acc.merge(m_benchDelta[i]);
  }
  m_unreadFrames = (folded ? m_unreadFrames : 0) + m_frames_since_publish;
  m_frames_since_publish = 0;
}
}
