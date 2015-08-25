/*!
 * \file Local.h
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

#include "Network/Protocol.h"

namespace OSSIA
{

struct Local : public Protocol
{
  
# pragma mark -
# pragma mark Life cycle
  
  Local();
  
# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(std::shared_ptr<Address>) const override;
  
  bool pushAddressValue(std::shared_ptr<Address>) const override;
  
  bool observeAddressValue(std::shared_ptr<Address>, bool) const override;
};

}

