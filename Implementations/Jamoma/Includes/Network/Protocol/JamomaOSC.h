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

#include "Network/JamomaProtocol.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaOSC : public OSC, public JamomaProtocol
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
  TTObject  mProtocol;
  
  string    mIp;
  int       mInPort;            /// the port that a remote device open to receive OSC messages
  int       mOutPort;           /// the port where a remote device sends OSC messages to give some feeback (like "echo")
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaOSC(string, int, int);
  
  ~JamomaOSC();
  
# pragma mark -
# pragma mark Accessors
  
  std::string getIp() override;
  
  Protocol & setIp(std::string);
  
  int getInPort() override;
  
  Protocol & setInPort(int);
  
  int getOutPort() override;
  
  Protocol & setOutPort(int);
  
# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(shared_ptr<Address>) const override;
  
  bool pushAddressValue(shared_ptr<Address>) const override;
  
  bool observeAddressValue(shared_ptr<Address>, bool) const override;
};
