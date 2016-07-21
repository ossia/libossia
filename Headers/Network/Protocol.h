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
#include <ossia_export.h>

namespace OSSIA
{

class Address;
class Node;
class NetworkLogger;

class OSSIA_EXPORT Protocol
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! destructor */
  virtual ~Protocol();

#if 0
# pragma mark -
# pragma mark Operation
#endif

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

  /*! Sets a logger that will record inbound and outbound messages.
   * \param A logger instance, or a null pointer to disable logging.
   */
  virtual void setLogger(std::shared_ptr<NetworkLogger>) = 0;

  /*! Get the logger instance.
   * @return A logger if it was set, or nullptr if there is none.
   */
  virtual std::shared_ptr<NetworkLogger> getLogger() const = 0;
};

OSSIA_EXPORT void CleanupProtocols();
}

