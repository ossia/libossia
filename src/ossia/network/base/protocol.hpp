#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/network/common/network_logger.hpp>

#include <ossia_export.h>

#include <future>
#include <memory>
#include <vector>

namespace ossia
{
class value;
namespace net
{
class parameter_base;
class node_base;
class device_base;
struct full_parameter_data;

class protocol_base;
struct message_origin_identifier
{
  ossia::net::protocol_base& protocol;
  uintptr_t identifier{};
};


/**
 * @brief The protocol_base class
 *
 * A protocol is used to implement remote communication.
 *
 * \see local_protocol
 * \see osc_protocol
 * \see minuit_protocol
 * \see oscquery_protocol
 * \see midi_protocol
 */
class OSSIA_EXPORT protocol_base
{
public:
  enum flags {
    SupportsMultiplex = (1 << 0)
  };

  explicit protocol_base(): m_flags{} { }
  explicit protocol_base(flags f): m_flags{f} { }
  protocol_base(const protocol_base&) = delete;
  protocol_base(protocol_base&&) = delete;
  protocol_base& operator=(const protocol_base&) = delete;
  protocol_base& operator=(protocol_base&&) = delete;

  virtual ~protocol_base();

  /**
   * @brief Pulls a value from the server synchronously.
   * @return True if the value was correctly retrieved.
   */
  virtual bool pull(parameter_base&) = 0;

  /**
   * @brief Pulls a value from the server asynchronously.
   * @return A future that will be set when the value is received.
   */
  virtual std::future<void> pull_async(parameter_base&);

  /**
   * @brief Request an update on a value.
   */
  virtual void request(parameter_base&);

  /**
   * @brief Send a value to the network.
   */
  virtual bool push(const parameter_base&, const ossia::value& v) = 0;
  virtual bool push(const parameter_base&, ossia::value&& v);
  bool push(const parameter_base& p);

  /**
   * @brief Send many values in one go if the protocol supports it
   */
  virtual bool
  push_bundle(const std::vector<const ossia::net::parameter_base*>&);

  /**
   * @brief Send a value to the network.
   */
  virtual bool push_raw(const full_parameter_data&) = 0;

  /**
   * @brief Send a value to the network.
   */
  virtual bool push_raw_bundle(const std::vector<full_parameter_data>&);

  /**
   * @brief Notify the network that a parameter should be listened to.
   *
   * In some protocols (Minuit, OSCQuery), this may send a message to the other
   * client
   * so that it regularly sends values of this parameter to the local computer.
   *
   * If a parameter is listened to, when a message is received,
   * parameter_base::setValue will be called,
   * which will notify the local signals.
   * Else, parameter_base::setValueQuiet will be called : the value will be
   * updated but the signals
   * won't be notified.
   */
  virtual bool observe(parameter_base&, bool) = 0;

  /**
   * @brief Begin observation without notifying the other computers.
   */
  virtual bool observe_quietly(parameter_base&, bool)
  {
    return false;
  }

  /**
   * @brief If the protocol supports it, request the namespace corresponding
   * to this node.
   */
  virtual std::future<void> update_async(node_base& node_base);

  /**
   * @brief If the protocol supports it, request the namespace corresponding
   * to this node. If the update takes too long, nodes may be dropped as there is a default timeout.
   */
  virtual bool update(node_base& node_base) = 0;

  /**
   * @brief It is mandatory to call this function from device implementations,
   * when
   * the protocol is set.
   */
  virtual void set_device(ossia::net::device_base& dev);

  //! Replace the loggers used
  virtual void set_logger(const network_logger& l)
  {
    m_logger = l;
  }

  virtual const network_logger& get_logger() const noexcept
  {
    return m_logger;
  }

  virtual void start_execution()
  {
  }
  virtual void stop_execution()
  {
  }
  virtual void stop()
  {
  }

  flags get_flags() const noexcept { return m_flags;}
  bool test_flag(flags f) const noexcept { return m_flags & f;}
protected:
  const flags m_flags{};
  network_logger m_logger;
};
}
}
