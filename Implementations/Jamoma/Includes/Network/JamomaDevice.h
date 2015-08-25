/*!
 * \file JamomaDevice.h
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

#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol.h"

#include "Network/JamomaNode.h"

#include "TTFoundation.h"
#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaDevice : public Device, public JamomaNode
{

private:

# pragma mark -
# pragma mark Implementation specific
  
  TTObject mApplicationManager;
  TTObject mApplication;

public:

# pragma mark -
# pragma mark Life cycle
  
  JamomaDevice(Protocol & protocol, TTObject applicationManager = TTObject(), TTObject application = TTObject(), TTNodeDirectoryPtr aDirectory = nullptr);

  ~JamomaDevice();

# pragma mark -
# pragma mark Network
  
  bool updateNamespace() override;
  
# pragma mark -
# pragma mark Implementation specific
  
  /* fill JamomaNode::mDevice member after contructor */
  void setDevice(shared_ptr<Device>);
};
