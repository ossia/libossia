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

#include <ossia/network/v1/Protocol.hpp>

#include <ossia/network/v1/detail/JamomaAddress.hpp>

#include "TTFoundation.hpp"
#include "TTModular.hpp"
#include <ossia/network/common/network_logger.hpp>

using namespace OSSIA;
using namespace std;

class OSSIA_EXPORT JamomaProtocol : public virtual Protocol
{

protected:

# pragma mark -
# pragma mark Implementation specific

  TTObject mApplicationManager;
  std::shared_ptr<NetworkLogger> mLogger;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaProtocol();

  void setLogger(std::shared_ptr<NetworkLogger>) override;
  std::shared_ptr<NetworkLogger> getLogger() const override;

    private:

  bool pullAddressValue(Address& address) const override;

  bool pushAddressValue(const Address& address) const override;

  bool observeAddressValue(std::shared_ptr<Address> address, bool enable) const override;
};
