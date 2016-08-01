#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/destination_index.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>

#include <functional>
#include <memory>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;
namespace net
{
class node_base;

using value_callback = std::function<void(const value&)>;

/**
 * @brief The address_base class
 *
 * An address holds attributes and values.
 * One can suscribe to modification of the value.
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

  virtual const ossia::net::node_base& getNode() const = 0;

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

  virtual value cloneValue(destination_index = {}) const = 0;
  virtual address_base& setValue(const value&) = 0;

  value fetchValue() const
  {
    pullValue();
    return cloneValue();
  }

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

  /**
   * @brief getTextualAddress
   * @return The address in the format device:/node/sub_node
   */
  virtual const std::string& getTextualAddress() const = 0;
};

/*!
 * \brief getAddressFromNode
 * \return the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string address_string_from_node(const ossia::net::node_base&);
}
}
