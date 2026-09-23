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
  std::array<float, max_meter_channels> sum_squares{};
  std::array<uint32_t, max_meter_channels / 32> clipped{};

  void clear() noexcept;
  void merge(const meter_levels& other) noexcept;
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

struct frame
{
  uint64_t publish_seq{};
  //! Frames executed since the arena started.
  uint64_t frames{};
  int32_t sample_rate{};
  std::vector<meter_slot> meters;
};

class OSSIA_EXPORT arena
{
public:
  //! Allocates everything the audio thread will ever touch.
  explicit arena(std::size_t meter_capacity);
  ~arena();

  arena(const arena&) = delete;
  arena& operator=(const arena&) = delete;

  [[nodiscard]] std::size_t meter_capacity() const noexcept { return m_sources.size(); }

  // Audio thread //

  //! Swaps the tap in: the previous one comes back in `tap`, so that it is
  //! released wherever the caller's closure is destroyed.
  void attach(
      std::size_t index, uint32_t generation, tap_kind kind,
      std::shared_ptr<meter_tap>& tap) noexcept;

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

  struct source
  {
    std::shared_ptr<meter_tap> tap;
    uint32_t generation{};
    tap_kind kind{};
  };

  // Audio thread only.
  std::vector<source> m_sources;
  std::vector<meter_slot> m_unread;
  frame m_staging;
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
