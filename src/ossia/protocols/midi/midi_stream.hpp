#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/lockfree_queue.hpp>

#include <libremidi/ump.hpp>

#include <optional>

namespace libremidi
{
class midi_in;
}

namespace ossia::net
{
class node_base;
}

namespace ossia::net::midi
{
/**
 * What the dataflow graph needs of a protocol before it can give one of its
 * nodes a MIDI port: somewhere to put what arrives, somewhere to send what
 * leaves, and the clock the two are timed against.
 *
 * Separate from midi_protocol because a protocol can build a tree of its own
 * and still carry MIDI -- a device description's controls are parameters, but
 * the port they are read from is the same wire.
 */
class OSSIA_EXPORT midi_stream
{
public:
  virtual ~midi_stream();

  /**
   * What arrived since the last tick: filled by the protocol's input callback,
   * drained by execution_state::get_new_values().
   *
   * Single producer, single consumer: the backend's callback thread and the
   * execution thread.
   */
  ossia::spsc_queue<libremidi::ump> messages;

  /**
   * The port messages are timed against, or nullptr when nothing is open for
   * input. A message's place in a tick is its timestamp measured from this.
   */
  virtual libremidi::midi_in* midi_in() const noexcept = 0;

  //! Start filling @ref messages. A protocol nothing reads as a stream should
  //! not pay to keep one.
  virtual void enable_registration() = 0;

  //! Send, for a MIDI port written to.
  virtual void push_value(const libremidi::ump&) = 0;

  /**
   * The channel @p n stands for, so that a port bound to it hears that channel
   * and not the other fifteen. std::nullopt for a node standing for the whole
   * stream, which is what the root of a device is.
   */
  virtual std::optional<int> stream_channel(const node_base& n) const noexcept;
};
}
