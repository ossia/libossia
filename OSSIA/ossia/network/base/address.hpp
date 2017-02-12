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
class OSSIA_EXPORT address_base : public callback_container<value_callback>
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

  /**
   * @brief pullValue
   *
   * Retrieve the current value over the network.
   * Not all protocols may provide this capability.
   *
   * This may be a blocking call.
   */
  virtual void pullValue() = 0;

  /**
   * @brief pullValueAsync
   *
   * Requests the current value over the network.
   * Not all protocols may provide this capability.
   *
   * This function returns a future that can be waited
   * upon by client code.
   */
  virtual std::future<void> pullValueAsync();

  /**
   * @brief requestValue
   *
   * Requests the current value over the network.
   * Not all protocols may provide this capability.
   *
   * This call may not block but there is no guarantee
   * that the value has been pulled when the call returns.
   */
  virtual void requestValue();

  /**
   * @brief pushValue
   *
   * Sets the value locally, and sends it to the network.
   */
  virtual address_base& pushValue(const value&) = 0;

  /**
   * @brief pushValue
   *
   * Sends the local value to the network
   */
  virtual address_base& pushValue() = 0;

  virtual value cloneValue() const = 0;

  /**
   * @brief cloneValueAtIndex Returns the sub-value at the index given by destination_index
   */
  value cloneValue(ossia::destination_index) const;

  /**
   * @brief cloneValueAtIndex Returns a tuple of sub-values matching the indexes
   */
  std::vector<ossia::value> cloneValue(const std::vector<ossia::destination_index>&) const;

  virtual address_base& setValue(const value&) = 0;

  //! Reimplement to provide a way that does not call the observers.
  virtual void setValueQuiet(const value& v) { setValue(v); }

  //! Pulls and clone the value.
  value fetchValue();

  virtual val_type getValueType() const = 0;
  virtual address_base& setValueType(val_type) = 0;

  virtual access_mode getAccessMode() const = 0;
  virtual address_base& setAccessMode(access_mode) = 0;

  virtual const domain& getDomain() const = 0;
  virtual address_base& setDomain(const domain&) = 0;

  virtual bounding_mode getBoundingMode() const = 0;
  virtual address_base& setBoundingMode(bounding_mode) = 0;

  virtual repetition_filter getRepetitionFilter() const = 0;
  virtual address_base& setRepetitionFilter(repetition_filter = repetition_filter::ON) = 0;
  virtual bool filterRepetition(const ossia::value& val) const { return false; } //! by default there is no filter

  virtual ossia::unit_t getUnit() const;
  virtual address_base& setUnit(const ossia::unit_t& v);
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

OSSIA_EXPORT ossia::value_with_unit get_value(const ossia::Destination& addr);
OSSIA_EXPORT void push_value(const ossia::Destination& addr, const ossia::value_with_unit&);
OSSIA_EXPORT std::ostream& operator<<(std::ostream&, const ossia::net::address_base&);
}


}


