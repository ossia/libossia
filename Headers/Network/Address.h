/*!
 * \file Address.h
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
#include <memory>
#include <string>
#include <functional>

#include "Network/AddressValue.h"
#include "Network/AddressDomain.h"

#include "Editor/Expression.h"
#include "Editor/ExpressionValue.h"

namespace OSSIA
{

class Device;

class Address : public ExpressionValue, public Expression
{

public:

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
  virtual ~Address() = default;

# pragma mark -
# pragma mark Value
  
  /*! update the address value internaly without updating client */
  virtual bool updateValue() const = 0;
  
  /*! get the address value
   \return #AddressValue* the value */
  virtual AddressValue * getValue() const = 0;
  
  /*! send a value to the address
   \param #AddressValue* the value */
  virtual bool sendValue(const AddressValue*) const = 0;

# pragma mark -
# pragma mark Network

  /*! get the device where the address is
   \note is this really needed ? how to provide it on implementation side ?
   \return std::shared_ptr<#Device> the device where the address is */
  virtual const std::shared_ptr<Device> & getDevice() const = 0;
  
  /*! \todo
   virtual Destination getDestination() const = 0;
   virtual Address & setDestination(Destination) = 0;
   */

# pragma mark -
# pragma mark Accessors
  
  /*! get the address type
   \return #AddressValue::Type of the address */
  virtual AddressValue::Type getValueType() const = 0;

  /*! get the address access mode
   \return #AccessMode of the address */
  virtual AccessMode getAccessMode() const = 0;
  
  /*! set the address access mode
   \param #AccessMode of the address */
  virtual Address & setAccessMode(AccessMode) = 0;
  
  /*! get the address domain 
   \return #AddressDomain of the address */
  virtual AddressDomain * getDomain() const = 0;
  
  /*! set the address domain
   \param #AddressDomain of the address */
  virtual Address & setDomain(AddressDomain *) = 0;
  
  /*! get the address bounding mode
   \todo multiple ?
   \return #BoundingMode of the address */
  virtual BoundingMode getBoundingMode() const = 0;
  
  /*! set the address bounding mode
   \todo multiple ?
   \param #BoundingMode of the address */
  virtual Address & setBoundingMode(BoundingMode) = 0;
  
  /*! get the address repetition filter status
   \return bool true is repetition filter is enabled */
  virtual bool getRepetitionFilter() const = 0;
  
  /*! set the address repetition filter status
   \param bool true is to enable repetition filter */
  virtual Address & setRepetitionFilter(bool = true) = 0;
  
# pragma mark -
# pragma mark Callback
  
  /*! to get the value back */
  using AddressValueCallback = std::function<void(const Address &, const AddressValue *)>;
  
  /*! get the address value callback function
   \return #AddressValueCallback function */
  AddressValueCallback getAddressValueCallback() const {return m_callback;};
  
  /*! set the address value callback function
   \param #AddressValueCallback function */
  void setAddressValueCallback(AddressValueCallback callback) {m_callback = callback;};
  
private:
  AddressValueCallback m_callback;
};

}

