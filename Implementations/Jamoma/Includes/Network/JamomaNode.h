/*!
 * \file JamomaNode.h
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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"

#include "Network/JamomaAddress.h"

#include "Misc/JamomaContainer.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaNode : public virtual Node, public enable_shared_from_this<JamomaNode>
{
  
protected:
  
# pragma mark -
# pragma mark Implementation specific
  
  TTNodeDirectory *         mDirectory{};
  TTNode *                  mNode{};
  
  weak_ptr<Device>          mDevice;
  weak_ptr<JamomaNode>      mParent;
  shared_ptr<Address>       mAddress;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaNode(TTNodeDirectory * aDirectory = nullptr, TTNode * aNode = nullptr, shared_ptr<Device> aDevice = nullptr, shared_ptr<JamomaNode> aParent = nullptr);
  
  ~JamomaNode();
  
# pragma mark -
# pragma mark Network
  
  shared_ptr<Device> getDevice() const override;
  
  shared_ptr<Node> getParent() const override;
  
# pragma mark -
# pragma mark Accessors
  
  string getName() const override;
  
  const shared_ptr<Address> & getAddress() const override;
  
# pragma mark -
# pragma mark Address
  
  shared_ptr<Address> createAddress(Value::Type type) override;
  
  bool removeAddress() override;
  
# pragma mark -
# pragma mark Children
  
  Container<Node>::iterator emplace(Container<Node>::const_iterator pos, string name) override;
  
# pragma mark -
# pragma mark Implementation specific
  
private:
  
  TTObject & getApplication() const;
  
  TTSymbol getApplicationType() const;
  
protected:
  
  void buildChildren();
};
