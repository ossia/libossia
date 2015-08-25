/*!
 * \file JamomaOSC.h
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

#include <string>

#include "Network/Protocol/OSC.h"

using namespace OSSIA;
using namespace std;

class JamomaOSC : public OSC
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
  string  mIp;
  int     mInPort;
  int     mOutPort;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaOSC(string, int, int);
  
  ~JamomaOSC();
  
# pragma mark -
# pragma mark Accessors
  
  string getIp() override;
  
  int getInPort() override;
  
  int getOutPort() override;
  
# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(shared_ptr<Address>) const override;
  
  bool pushAddressValue(shared_ptr<Address>) const override;
  
  bool observeAddressValue(shared_ptr<Address>, bool) const override;
};
