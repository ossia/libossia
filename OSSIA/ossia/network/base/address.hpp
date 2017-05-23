#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain_fwd.hpp>
#include <ossia/editor/dataspace/dataspace_fwd.hpp>
#include <ossia/editor/value/destination.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/detail/optional.hpp>
#include <functional>
#include <vector>
#include <memory>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>
#include <future>

namespace ossia
{
class value;
namespace net
{
class node_base;

/**
 * @brief The address_base class
 *
 * An address holds attributes and values.
 * One can subscribe to modification of the value.
 *
 * \see generic_address
 *
 */
class OSSIA_EXPORT address_base :
    public callback_container<value_callback>
{
public:
  address_base() = default;
  address_base(const address_base&) = delete;
  address_base(address_base&&) = delete;
  address_base& operator=(const address_base&) = delete;
  address_base& operator=(address_base&&) = delete;

  using callback_index = callback_container<value_callback>::iterator;
  virtual ~address_base();

  virtual ossia::net::node_base& getNode() const = 0;

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
  virtual address_base& push_value(const ossia::value&) = 0;
  virtual address_base& push_value(ossia::value&&) = 0;


  /// Value setters ///
  //! Sends the local value to the network
  virtual address_base& push_value() = 0;

  //! Returns the sub-value at the index given by destination_index
  ossia::value value(ossia::destination_index) const;

  //! Returns a tuple of sub-values matching the indexes
  std::vector<ossia::value> value(const std::vector<ossia::destination_index>&) const;

  virtual address_base& set_value(const ossia::value&) = 0;
  virtual address_base& set_value(ossia::value&&) = 0;

  //! Reimplement to provide a way that does not call the observers.
  virtual void set_value_quiet(const ossia::value& v) { set_value(v); }
  virtual void set_value_quiet(ossia::value&& v) { set_value(std::move(v)); }


  virtual val_type get_value_type() const = 0;
  virtual address_base& set_value_type(val_type) = 0;

  virtual access_mode get_access() const = 0;
  virtual address_base& set_access(access_mode) = 0;

  virtual const domain& get_domain() const = 0;
  virtual address_base& set_domain(const domain&) = 0;

  virtual bounding_mode get_bounding() const = 0;
  virtual address_base& set_bounding(bounding_mode) = 0;

  virtual repetition_filter get_repetition_filter() const = 0;
  virtual address_base& set_repetition_filter(repetition_filter = repetition_filter::ON) = 0;
  virtual bool filter_repetition(const ossia::value& val) const { return false; } //! by default there is no filter

  virtual ossia::unit_t get_unit() const;
  virtual address_base& set_unit(const ossia::unit_t& v);

  virtual bool get_muted() const;
  virtual address_base& set_muted(bool);
  virtual bool get_critical() const;
  virtual address_base& set_critical(bool);
};

inline bool operator==(const address_base& lhs, const address_base& rhs)
{ return &lhs == &rhs; }
inline bool operator!=(const address_base& lhs, const address_base& rhs)
{ return &lhs != &rhs; }


/*!
 * \brief getAddressFromNode
 * \return the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string address_string_from_node(const ossia::net::node_base&);
OSSIA_EXPORT std::string address_string_from_node(const ossia::net::address_base&);

//! Address without the 'device:' part.
OSSIA_EXPORT std::string osc_address_string(const ossia::net::address_base&);
OSSIA_EXPORT std::string osc_address_string(const ossia::net::node_base&);

OSSIA_EXPORT std::string osc_address_string_with_device(const ossia::net::address_base&);
OSSIA_EXPORT std::string osc_address_string_with_device(const ossia::net::node_base&);

OSSIA_EXPORT ossia::value_with_unit get_value(const ossia::Destination& addr);
OSSIA_EXPORT void push_value(const ossia::Destination& addr, const ossia::value_with_unit&);
OSSIA_EXPORT std::ostream& operator<<(std::ostream&, const ossia::net::address_base&);
}


}


