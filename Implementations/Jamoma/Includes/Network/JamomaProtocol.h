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

#include "Network/Protocol.h"

#include "Network/JamomaAddress.h"

#include "TTFoundation.h"
#include "TTModular.h"
#include "Network/NetworkLogger.h"

using namespace OSSIA;
using namespace std;

class JamomaProtocol : public virtual Protocol
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

  bool pullAddressValue(Address& address) const;

  bool pushAddressValue(const Address& address) const;

  bool observeAddressValue(std::shared_ptr<Address> address, bool enable) const;
};
