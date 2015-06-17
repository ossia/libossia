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

#include "Editor/Domain.h"
#include "Editor/Expression.h"
#include "Editor/Value.h"

namespace OSSIA
{

class Device;

class Address : public Expression
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
   \return const #Value* the value */
  virtual const Value * getValue() const = 0;
  
  /*! send a value to the address
   \param #Value* the value */
  virtual bool sendValue(const Value*) const = 0;

# pragma mark -
# pragma mark Network

  /*! get the device where the address is
   \note is this really needed ? how to provide it on implementation side ?
   \return std::shared_ptr<#Device> the device where the address is */
  virtual const std::shared_ptr<Device> & getDevice() const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the address type
   \return #Value::Type of the address */
  virtual Value::Type getValueType() const = 0;

  /*! get the address access mode
   \return #AccessMode of the address */
  virtual AccessMode getAccessMode() const = 0;
  
  /*! set the address access mode
   \param #AccessMode of the address */
  virtual Address & setAccessMode(AccessMode) = 0;
  
  /*! get the address domain 
   \return #Domain of the address */
  virtual const std::shared_ptr<Domain> & getDomain() const = 0;
  
  /*! set the address domain
   \param #Domain of the address */
  virtual Address & setDomain(std::shared_ptr<Domain>) = 0;
  
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
  using ValueCallback = std::function<void(const Value *)>;
  
  /*! get the address value callback function
   \return #ValueCallback function */
  virtual ValueCallback getValueCallback() const = 0;
  
  /*! set the address value callback function
   \param #ValueCallback function */
  virtual void setValueCallback(ValueCallback callback) = 0;
  
protected:
  ValueCallback m_callback;
};
  
}

