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
#include "Network/Node.h"

namespace OSSIA
{

class Address;
class Node;

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
  virtual ~Protocol();

# pragma mark -
# pragma mark Accessors

  /* get the type of the segment
   \return #Type */
  virtual Type getType() const = 0;

# pragma mark -
# pragma mark Operation

  /*! send a request to update an address value
   \details some protocols cannot do this operation
   \param #Address the address to pull
   \return bool true if the operation succeeded */
  virtual bool pullAddressValue(Address&) const = 0;

  /*! send a message to set an address value
   \details some protocols cannot do this operation
   \param #Address the address to push
   \return bool true if the operation succeeded */
  virtual bool pushAddressValue(const Address&) const = 0;

  /*! send a request to enable/disable observation on an address value
   \details some protocols cannot do this operation, some others observe everything
   \param shared_ptr<#Address> the address to observe
   \return bool true if the operation succeeded */
  virtual bool observeAddressValue(std::shared_ptr<Address>, bool) const = 0;

  /*! send a request to update the children of a node
   \details some protocols cannot do this operation
   \param #Node the node to update
   \return bool true if the operation succeeded */
  virtual bool updateChildren(Node& node) const = 0;
};

void CleanupProtocols();
}

