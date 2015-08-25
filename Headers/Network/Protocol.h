/*!
 * \file Protocol.h
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

#include <memory>

#include "Network/Address.h"

namespace OSSIA
{

class Protocol
{
  
public:
  
# pragma mark -
# pragma mark Enumerations
  
  /*! type of protocol */
  enum class Type
  {
    LOCAL,
    MIDI,
    MINUIT,
    OSC
  };
  
# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~Protocol() = default;
  
# pragma mark -
# pragma mark Accessors
  
  /* get the type of the segment
   \return #Type */
  virtual Type getType() const = 0;
  
# pragma mark -
# pragma mark Operation
  
  /*! send a request to update an address value
   \details some protocols cannot do this operation
   \return bool true if the operation succeeded */
  virtual bool pullAddressValue(std::shared_ptr<Address>) const = 0;
  
  /*! send a message to set an address value
   \details some protocols cannot do this operation
   \return bool true if the operation succeeded */
  virtual bool pushAddressValue(std::shared_ptr<Address>) const = 0;
  
  /*! send a request to enable/disable observation on an address value
   \details some protocols cannot do this operation, some others observe everything
   \return bool true if the operation succeeded */
  virtual bool observeAddressValue(std::shared_ptr<Address>, bool) const = 0;
  
};
  
}

