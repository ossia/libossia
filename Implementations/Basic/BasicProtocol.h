/*!
 * \file JamomaProtocol.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Network/NetworkLogger.h"
#include <memory>
namespace OSSIA
{
class Address2;
class Node2;
class NetworkLogger;

class OSSIA_EXPORT Protocol2
{
public:
  virtual ~Protocol2();

  virtual bool pullAddressValue(Address2&) const = 0;
  virtual bool pushAddressValue(const Address2&) const = 0;

  // Does not have ownership
  virtual bool observeAddressValue(Address2&, bool) const = 0;

  virtual bool updateChildren(Node2& node) const = 0;

  virtual void setLogger(std::shared_ptr<NetworkLogger>) = 0;
  virtual std::shared_ptr<NetworkLogger> getLogger() const = 0;
};

OSSIA_EXPORT void CleanupProtocols();
}



namespace impl
{

class OSSIA_EXPORT BasicProtocol : public virtual OSSIA::Protocol2
{

protected:
  std::shared_ptr<OSSIA::NetworkLogger> mLogger;

public:
  BasicProtocol();

  void setLogger(std::shared_ptr<OSSIA::NetworkLogger>) override;
  std::shared_ptr<OSSIA::NetworkLogger> getLogger() const override;

    private:

  bool pullAddressValue(OSSIA::Address2& address) const override;

  bool pushAddressValue(const OSSIA::Address2& address) const override;

  bool observeAddressValue(OSSIA::Address2& address, bool enable) const override;
};

}
