/*!
 * \file Address.h
 *
 * \defgroup Network
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <string>
#include <functional>

#include "Editor/Domain.h"
#include "Editor/Expression.h"
#include "Editor/Value.h"

namespace OSSIA
{

class Node;

/*! to get the value back
 \param the returned value */
using ValueCallback = std::function<void(const Value *)>;

class Address : public CallbackContainer<ValueCallback>
{

public:
  
# pragma mark -
# pragma mark Definitions
  
  using iterator = typename CallbackContainer<ValueCallback>::iterator;
  
# pragma mark -
# pragma mark Enumerations

  /*! operation allowed on address */
  enum class AccessMode
  {
    GET,
    SET,
    BI
  };

  /*! address behaviors at crossing domain boundaries time */
  enum class BoundingMode
  {
    FREE,
    CLIP,
    WRAP,
    FOLD
  };

# pragma mark -
# pragma mark Life cycle

  /*! destructor */
  virtual ~Address();

# pragma mark -
# pragma mark Network

  /*! get the node where the address is
   \return std::shared_ptr<#Node> the node where the address is */
  virtual const std::shared_ptr<Node> getNode() const = 0;

  /*! pull and return the address value from a device using its protocol
   \see Protocol::pullAddressValue method
   \return const #Value* the value */
  virtual const Value * pullValue() = 0;

  /*! set then push the address value to a device using its protocol
   \see Protocol::pushAddressValue method
   \param const #Value* the value (push the current value if no argument)
   \return #Address the address */
  virtual Address & pushValue(const Value* = nullptr) = 0;

# pragma mark -
# pragma mark Accessors

  /*! get the address value
   \details call pullValue if you need to sync the value with the device
   \return const #Value* the value */
  virtual const Value * getValue() const = 0;

  /*! set the address value
   \note call pushValue if you need to sync the value with the device
   \param const #Value* the value
   \note it is possible to set the value using a #Value::Type::Destination to query the value from another address
   \return #Address the address */
  virtual Address & setValue(const Value*) = 0;

  /*! get the address type
   \return #Value::Type of the address */
  virtual Value::Type getValueType() const = 0;
  
  /*! set the address type
   \param #Value::Type of the address
   \return #Address the address */
  virtual Address & setValueType(Value::Type) = 0;

  /*! get the address access mode
   \return #AccessMode of the address */
  virtual AccessMode getAccessMode() const = 0;

  /*! set the address access mode
   \param #AccessMode of the address
   \return #Address the address */
  virtual Address & setAccessMode(AccessMode) = 0;

  /*! get the address domain
   \return #Domain of the address */
  virtual const std::shared_ptr<Domain> & getDomain() const = 0;

  /*! set the address domain
   \param #Domain of the address
   \return #Address the address */
  virtual Address & setDomain(std::shared_ptr<Domain>) = 0;

  /*! get the address bounding mode
   \todo multiple ?
   \return #BoundingMode of the address */
  virtual BoundingMode getBoundingMode() const = 0;

  /*! set the address bounding mode
   \todo multiple ?
   \param #BoundingMode of the address
   \return #Address the address */
  virtual Address & setBoundingMode(BoundingMode) = 0;

  /*! get the address repetition filter status
   \return bool true is repetition filter is enabled */
  virtual bool getRepetitionFilter() const = 0;

  /*! set the address repetition filter status
   \param bool true is to enable repetition filter
   \return #Address the address */
  virtual Address & setRepetitionFilter(bool = true) = 0;
};
}

