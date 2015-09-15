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

using namespace OSSIA;
using namespace std;

class JamomaProtocol : public virtual Protocol
{
  
protected:
  
# pragma mark -
# pragma mark Implementation specific
  
  TTObject mApplicationManager;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaProtocol();
  
# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(Address&) const override;
  
  bool pushAddressValue(const Address&) const override;
  
  bool observeAddressValue(shared_ptr<Address>, bool) const override;
};
