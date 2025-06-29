#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/destination_index.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/detail/std_fwd.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/dataspace/dataspace_fwd.hpp>
#include <ossia/network/domain/domain_fwd.hpp>
#include <ossia/network/value/destination.hpp>

#include <nano_signal_slot.hpp>

#include <functional>
#include <memory>
#include <vector>
#include <version>
OSSIA_STD_BEGIN_NAMESPACE
template <typename T>
class future;
OSSIA_STD_END_NAMESPACE

/*
extern template class ossia::callback_container<ossia::value_callback>;
extern template class std::future<void>;
*/
namespace ossia
{
class value;
namespace net
{
class node_base;
class protocol_base;
struct full_parameter_data;

/**
 * @brief The parameter_base class
 *
 * A parameter holds attributes and values.
 * One can subscribe to modification of the value.
 *
 * \see generic_parameter
 * \see node_attributes.hpp for description of the attributes.
 */
class OSSIA_EXPORT parameter_base : public callback_container<value_callback>
{
public:
  explicit parameter_base(ossia::net::node_base& n)
      : m_node{n}
  {
  }
  explicit parameter_base(const parameter_base&) = delete;
  explicit parameter_base(parameter_base&&) = delete;
  parameter_base& operator=(const parameter_base&) = delete;
  parameter_base& operator=(parameter_base&&) = delete;

  using callback_index = callback_container<value_callback>::iterator;
  virtual ~parameter_base();

  ossia::net::node_base& get_node() const noexcept { return m_node; }
  virtual ossia::net::protocol_base& get_protocol() const noexcept;

  /// Value getters ///
  /**
   * @brief pull_value
   *
   * Retrieve the current value over the network.
   * Not all protocols may provide this capability.
   *
   * This may be a blocking call.
   */
  virtual void pull_value() = 0;

  /**
   * @brief pull_value_async
   *
   * Requests the current value over the network.
   * Not all protocols may provide this capability.
   *
   * This function returns a future that can be waited
   * upon by client code.
   */
  virtual std::future<void> pull_value_async();

  /**
   * @brief request_value
   *
   * Requests the current value over the network.
   * Not all protocols may provide this capability.
   *
   * This call may not block but there is no guarantee
   * that the value has been pulled when the call returns.
   *
   * However, the callback will be called when the value is received.
   */
  virtual void request_value();

  //! Clone the current value without any network request
  virtual ossia::value value() const = 0;

  //! Pulls and clone the value.
  ossia::value fetch_value();

  //! Sets the value locally, and sends it to the network.
  virtual parameter_base& push_value(const ossia::value&) = 0;
  virtual parameter_base& push_value(ossia::value&&) = 0;
  void push_value_quiet(const ossia::value& v)
  {
    set_value_quiet(v);
    push_value();
  }
  void push_value_quiet(ossia::value&& v)
  {
    set_value_quiet(std::move(v));
    push_value();
  }

  /// Value setters ///
  //! Sends the local value to the network
  virtual parameter_base& push_value() = 0;

  //! Returns the sub-value at the index given by destination_index
  ossia::value value(ossia::destination_index) const;

  //! Returns a list of sub-values matching the indexes
  std::vector<ossia::value> value(const std::vector<ossia::destination_index>&) const;

  virtual ossia::value set_value(const ossia::value&) = 0;
  virtual ossia::value set_value(ossia::value&&) = 0;

  //! Reimplement to provide a way that does not call the observers.
  virtual ossia::value set_value_quiet(const ossia::value& v) { return set_value(v); }
  virtual ossia::value set_value_quiet(ossia::value&& v)
  {
    return set_value(std::move(v));
  }

  std::optional<ossia::value> get_default_value() const noexcept;
  void set_default_value(const ossia::value& v);

  virtual val_type get_value_type() const noexcept = 0;
  virtual parameter_base& set_value_type(val_type) = 0;

  virtual access_mode get_access() const noexcept = 0;
  virtual parameter_base& set_access(access_mode) = 0;

  virtual const domain& get_domain() const noexcept = 0;
  virtual parameter_base& set_domain(const domain&) = 0;

  virtual bounding_mode get_bounding() const noexcept = 0;
  virtual parameter_base& set_bounding(bounding_mode) = 0;

  repetition_filter get_repetition_filter() const noexcept;
  parameter_base& set_repetition_filter(repetition_filter = repetition_filter::ON);
  virtual bool filter_value(const ossia::value& val) const noexcept
  {
    return m_disabled || m_muted;
  } //! by default there is no filter

  const ossia::unit_t& get_unit() const noexcept { return m_unit; }
  virtual parameter_base& set_unit(const ossia::unit_t& v);

  // Shared across the network
  bool get_disabled() const noexcept;
  parameter_base& set_disabled(bool);

  // Local only
  bool get_muted() const noexcept;
  parameter_base& set_muted(bool);

  bool get_critical() const noexcept;
  parameter_base& set_critical(bool v);

  parameter_type get_type() const noexcept;

protected:
  ossia::net::node_base& m_node;
  unit_t m_unit;
  bool m_critical{};
  bool m_disabled{};
  bool m_muted{};
  ossia::repetition_filter m_repetitionFilter{ossia::repetition_filter::OFF};
  parameter_type m_type{};
};

struct OSSIA_EXPORT parameter_alphabetical_sort
{
  bool operator()(
      const ossia::net::parameter_base* lhs,
      const ossia::net::parameter_base* rhs) const noexcept;
};

inline bool operator==(const parameter_base& lhs, const parameter_base& rhs)
{
  return &lhs == &rhs;
}
inline bool operator!=(const parameter_base& lhs, const parameter_base& rhs)
{
  return &lhs != &rhs;
}
}
}
