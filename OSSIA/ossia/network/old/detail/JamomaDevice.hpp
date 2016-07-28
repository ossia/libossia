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

#include <ossia/network/base/Device.hpp>
#include <ossia/network/base/Node.hpp>
#include <ossia/network/v1/Protocol.hpp>

#include <ossia/network/v1/detail/JamomaNode.hpp>

#include "TTFoundation.h"
#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaDevice final : public Device, public JamomaNode
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<Protocol>  mProtocol;

  TTObject              mApplicationManager;
  TTObject              mApplication;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaDevice(std::shared_ptr<Protocol> protocol, TTObject applicationManager = TTObject(), TTObject application = TTObject(), TTNodeDirectoryPtr aDirectory = nullptr);

  ~JamomaDevice();

# pragma mark -
# pragma mark Accessors

  string getName() const override;

  Node & setName(std::string) override;

  std::shared_ptr<Protocol> getProtocol() const override;

  //! \deprecated use Protocol::updateChildren
  bool updateNamespace() override;

# pragma mark -
# pragma mark Implementation specific

  /* fill JamomaNode::mDevice member after contructor */
  void setDevice(std::shared_ptr<Device>);
};
