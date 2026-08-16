#pragma once

// PipeWire-facing helpers with no PipeWire dependency, so that the process
// cycle policy is unit-testable without a daemon.
//
// Background: the quantum (block size) of a PipeWire graph is a global,
// per-driver negotiation. Even when a node sets node.force-quantum +
// node.lock-quantum, the graph can legitimately run at a different quantum:
//  - transiently, between the node joining and the driver picking up
//    clock.target_duration (one cycle minimum, more when drivers move);
//  - when the global `clock.force-quantum` setting is active (it overrides
//    every per-node force);
//  - when another node's force-quantum has a newer stamp (last-write-wins
//    between force-quantum nodes on the same driver);
//  - when the value is clamped by `clock.quantum-floor` / `clock.quantum-limit`
//    (e.g. min-quantum is raised to 1024 inside VMs by the default config).
// A client that refuses to process such cycles outputs silence forever
// (its output io stays in NEED_DATA); the only correct behaviour is to adapt.

#include <cstddef>
#include <cstdint>

namespace ossia::pipewire
{

// Tracks the block size (or sample rate) seen on each process cycle against
// the configured value, and turns changes into loggable transitions so the
// realtime callback logs once per reconfiguration instead of once per cycle.
struct quantum_tracker
{
  std::uint32_t expected{};
  std::uint32_t last_seen{};

  enum class event : std::uint8_t
  {
    steady,    // same value as the previous cycle
    matched,   // first cycle, at the expected value
    mismatch,  // value changed to something != expected: warn
    recovered, // back to the expected value after a mismatch: info
  };

  constexpr event observe(std::uint32_t value) noexcept
  {
    if (value == last_seen)
      return event::steady;
    const auto prev = last_seen;
    last_seen = value;
    if (value == expected)
      return prev == 0 ? event::matched : event::recovered;
    return event::mismatch;
  }
};

// Splits a cycle of nframes into consecutive chunks of at most max_frames,
// invoking f(offset, frames) for each. The engine's buffers are sized for
// the configured block size, so a larger graph quantum must be processed
// in slices instead of overrunning them.
template <typename F>
constexpr void
for_each_chunk(std::uint32_t nframes, std::uint32_t max_frames, F&& f)
{
  if (max_frames == 0)
    return;
  for (std::uint32_t offset = 0; offset < nframes;)
  {
    const std::uint32_t n
        = nframes - offset < max_frames ? nframes - offset : max_frames;
    f(offset, n);
    offset += n;
  }
}

// Builds the per-chunk channel pointer array: channels whose cycle buffer
// exists advance by offset, missing ones fall back to scratch. Inputs and
// outputs must use *distinct* scratch storage: sharing one dummy buffer
// feeds each tick's discarded output back into the missing inputs.
inline void assign_chunk_pointers(
    float* const* cycle, float** chunk, std::size_t channels,
    std::uint32_t offset, float* scratch) noexcept
{
  for (std::size_t i = 0; i < channels; ++i)
    chunk[i] = cycle[i] ? cycle[i] + offset : scratch;
}

}
