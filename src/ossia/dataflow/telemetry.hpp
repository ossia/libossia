#pragma once
#include <ossia/audio/audio_tick.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/detail/triple_buffer.hpp>

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

/**
 * Informative data going from the execution back to the user interface.
 *
 * The audio thread never allocates, never locks and never loses a peak:
 * - every metering point (a tap) accumulates on its own writer thread;
 * - once per publication period, the tick thread gathers all taps into one
 *   frame at a fixed slot per tap and hands it over through a triple buffer;
 * - levels the interface has not read yet are merged into the next frame
 *   instead of being dropped, so the interface can read at any rate.
 *
 * Slots are allocated by the interface, which pairs each index with a
 * generation: a reader only trusts a slot whose generation it expects.
 */
namespace ossia::telemetry
{
inline constexpr std::size_t max_meter_channels = 128;

//! Levels accumulated over a span of frames.
struct OSSIA_EXPORT meter_levels
{
  //! Widest signal seen. 0 with ticks > 0: the tap ran and carried nothing.
  uint32_t channels{};
  //! How many times the tap was fed. 0: it did not run.
  uint32_t ticks{};
  uint64_t frames{};
  std::array<float, max_meter_channels> peak{};
  //! Per channel, over the frames that channel was there for.
  std::array<double, max_meter_channels> sum_squares{};
  std::array<uint32_t, max_meter_channels> channel_frames{};
  std::array<uint32_t, max_meter_channels / 32> clipped{};

  void clear() noexcept;
  void merge(const meter_levels& other) noexcept;
  //! Copies what is in use of `other`: its channels, not the whole capacity.
  void assign(const meter_levels& other) noexcept;
  void accumulate(const ossia::audio_vector& chans) noexcept;
  void accumulate(const float* const* chans, int count, std::size_t frames) noexcept;

  [[nodiscard]] bool is_clipped(std::size_t chan) const noexcept
  {
    return chan < max_meter_channels && (clipped[chan / 32] >> (chan % 32)) & 1u;
  }
  [[nodiscard]] float rms(std::size_t chan) const noexcept;
};

//! One metering point. Only its writer thread touches it while it is attached.
struct meter_tap
{
  meter_levels pending;
};

//! Time a node spent running over a span of frames.
struct bench_levels
{
  uint64_t ns{};
  uint64_t max_ns{};
  uint32_t runs{};

  void clear() noexcept { *this = {}; }
  void add(int64_t t) noexcept
  {
    const auto n = uint64_t(t > 0 ? t : 0);
    ns += n;
    max_ns = n > max_ns ? n : max_ns;
    runs++;
  }
  void merge(const bench_levels& o) noexcept
  {
    ns += o.ns;
    max_ns = o.max_ns > max_ns ? o.max_ns : max_ns;
    runs += o.runs;
  }
};

//! One timed node, fed by the graph's executors.
struct bench_tap
{
  bench_levels pending;
};

//! Where something that runs in time is: written by the tick, the latest
//! value wins.
struct playhead_tap
{
  int64_t date{};
  bool running{};
};

struct playhead_slot
{
  uint32_t generation{};
  int64_t date{};
  bool running{};
};

enum class tap_kind : uint8_t
{
  none,
  node,             //!< fed by the graph, e.g. an audio outlet's post_process
  hardware_inputs,  //!< fed by the tick from the driver's inputs
  hardware_outputs, //!< fed by the tick from the driver's outputs, after the mix
};

struct meter_slot
{
  uint32_t generation{};
  meter_levels levels;
};

struct bench_slot
{
  uint32_t generation{};
  bench_levels levels;
};

struct frame
{
  uint64_t publish_seq{};
  //! Frames executed since the arena started.
  uint64_t frames{};
  //! Frames the slots cover: since the previous frame that was read.
  uint64_t window_frames{};
  int32_t sample_rate{};
  std::vector<meter_slot> meters;
  std::vector<bench_slot> benches;
  std::vector<playhead_slot> playheads;

  //! Share of the real time a bench slot's node took to run, 1 being all of
  //! it.
  [[nodiscard]] double load(const bench_slot& b) const noexcept
  {
    if(window_frames == 0 || sample_rate <= 0)
      return 0.;
    return double(b.levels.ns) * sample_rate / (1e9 * double(window_frames));
  }
};

class OSSIA_EXPORT arena
{
public:
  //! Allocates everything the audio thread will ever touch.
  explicit arena(
      std::size_t meter_capacity, std::size_t bench_capacity = 0,
      std::size_t playhead_capacity = 0);
  ~arena();

  arena(const arena&) = delete;
  arena& operator=(const arena&) = delete;

  [[nodiscard]] std::size_t meter_capacity() const noexcept { return m_sources.size(); }
  [[nodiscard]] std::size_t bench_capacity() const noexcept
  {
    return m_benchSources.size();
  }
  [[nodiscard]] std::size_t playhead_capacity() const noexcept
  {
    return m_playheadSources.size();
  }

  // Audio thread //

  //! Swaps the tap in: the previous one comes back in `tap`, so that it is
  //! released wherever the caller's closure is destroyed.
  void attach(
      std::size_t index, uint32_t generation, tap_kind kind,
      std::shared_ptr<meter_tap>& tap) noexcept;
  //! Same, for a timed node.
  void attach_bench(
      std::size_t index, uint32_t generation, std::shared_ptr<bench_tap>& tap) noexcept;
  //! Same, for a playhead.
  void attach_playhead(
      std::size_t index, uint32_t generation,
      std::shared_ptr<playhead_tap>& tap) noexcept;

  //! Feeds the hardware taps from the driver's buffers. Call after the mix.
  void accumulate_hardware(const ossia::audio_tick_state& st) noexcept;

  //! Counts the frames of this tick and publishes once a period has elapsed.
  void tick(std::size_t frames, int sample_rate) noexcept;

  // Any thread //

  //! Frames between two publications, e.g. the UI refresh period.
  void set_publish_interval(uint64_t frames) noexcept
  {
    m_publish_interval.store(frames, std::memory_order_relaxed);
  }

  // UI thread //

  //! Takes the latest frame, if one was published since the last call.
  bool consume() noexcept { return m_buffer.consume(m_read); }
  [[nodiscard]] const frame& latest() const noexcept { return m_read; }

private:
  void publish() noexcept;
  //! Fills the buffer's write slot with what came since the previous
  //! publication, plus what was not read yet if `with_unread`.
  void stage(bool with_unread) noexcept;

  struct source
  {
    std::shared_ptr<meter_tap> tap;
    uint32_t generation{};
    tap_kind kind{};
  };

  struct bench_source
  {
    std::shared_ptr<bench_tap> tap;
    uint32_t generation{};
  };

  // Audio thread only. What came since the previous publication, and what the
  // interface has not read yet besides.
  std::vector<source> m_sources;
  std::vector<meter_slot> m_unread;
  std::vector<meter_levels> m_delta;
  std::vector<bench_source> m_benchSources;
  std::vector<bench_slot> m_benchUnread;
  std::vector<bench_levels> m_benchDelta;

  struct playhead_source
  {
    std::shared_ptr<playhead_tap> tap;
    uint32_t generation{};
  };
  std::vector<playhead_source> m_playheadSources;
  uint64_t m_unreadFrames{};
  uint64_t m_frames{};
  uint64_t m_frames_since_publish{};
  uint64_t m_seq{};
  int32_t m_sample_rate{};

  std::atomic<uint64_t> m_publish_interval{4096};
  ossia::triple_buffer<frame> m_buffer;

  // UI thread only.
  frame m_read;
};
}
