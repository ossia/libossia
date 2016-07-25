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

#include "Protocol.hpp"
#include <Network/NetworkLogger.h>
#include <memory>

namespace impl
{
class OSSIA_EXPORT BasicProtocol : public virtual OSSIA::v2::Protocol2
{

protected:
  std::shared_ptr<OSSIA::NetworkLogger> mLogger;

public:
  BasicProtocol();

  void setLogger(std::shared_ptr<OSSIA::NetworkLogger>) override;
  std::shared_ptr<OSSIA::NetworkLogger> getLogger() const override;

    private:

  bool pullAddressValue(OSSIA::v2::Address2& address) const override;

  bool pushAddressValue(const OSSIA::v2::Address2& address) const override;

  bool observeAddressValue(OSSIA::v2::Address2& address, bool enable) const override;
};

}
