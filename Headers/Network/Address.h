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
#include "Network/AddressValue.h"

#include <memory>
#include <string>

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
  
  /*! operation allowed on value */
  enum class AccessMode
  {
    GET,
    SET,
    BI
  };
  
  /*! value behaviors at crossing domain boundaries time */
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
  
  /*! update the value internaly without updating client */
  virtual bool updateValue() const = 0;
  
  /*! get the value 
   \return #AddressValue* the value */
  virtual AddressValue * getValue() const = 0;
  
  /*! send a value
   \param #AddressValue* the value */
  virtual bool sendValue(const AddressValue*) const = 0;

# pragma mark -
# pragma mark Network

  /*! get the device where the address value is
   \note is this really needed ? how to provide it on implementation side ?
   \return std::shared_ptr<#Device> the device where the address value is */
  virtual const std::shared_ptr<Device> & getDevice() const = 0;
  
  /*! \todo
   virtual Destination getDestination() const = 0;
   virtual Address & setDestination(Destination) = 0;
   */

# pragma mark -
# pragma mark Accessors
  
  /*! get the value type
   \return #AddressValue::Type of the value */
  virtual AddressValue::Type getValueType() const = 0;

  /*! get the value access mode
   \return #AccessMode of the value */
  virtual AccessMode getAccessMode() const = 0;
  
  /*! set the value access mode
   \param #AccessMode of the value */
  virtual Address & setAccessMode(AccessMode) = 0;
  
  /*! \todo
  virtual Domain getDomain() const = 0;
  virtual Address & setDomain(Domain) = 0;
  */
  
  /*! get the value bounding mode
   \todo multiple ?
   \return #BoundingMode of the value */
  virtual BoundingMode getBoundingMode() const = 0;
  
  /*! set the value bounding mode
   \todo multiple ?
   \param #BoundingMode of the value */
  virtual Address & setBoundingMode(BoundingMode) = 0;
  
  /*! get the value repetition filter status
   \return bool true is repetition filter is enabled */
  virtual bool getRepetitionFilter() const = 0;
  
  /*! set the value repetition filter status
   \param bool true is to enable repetition filter */
  virtual Address & setRepetitionFilter(bool = true) = 0;
};

}

