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

#include <ossia/network/base/address.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/v1/Protocol.hpp>

#include <ossia/network/v1/detail/JamomaAddress.hpp>

#include "TTModular.h"

#include <map>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaProtocol;

class JamomaNode : public virtual Node, public std::enable_shared_from_this<JamomaNode>
{

protected:

# pragma mark -
# pragma mark Implementation specific

  TTNodeDirectory *       mDirectory{};
  TTNode *                mNode{};
  TTObject                mObject;

  weak_ptr<Device>        mDevice;
  weak_ptr<JamomaNode>    mParent;
  std::shared_ptr<Address>     mAddress;

  bool                    mIsDevice{false};

  map<shared_ptr<Node>, Node::iterator> mChildNodeChangeCallbackIndexes;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaNode(TTNodeDirectory * aDirectory = nullptr, TTNode * aNode = nullptr, std::shared_ptr<Device> aDevice = nullptr, std::shared_ptr<JamomaNode> aParent = nullptr);

  ~JamomaNode();

# pragma mark -
# pragma mark Network

  std::shared_ptr<Device> getDevice() const override;

  std::shared_ptr<Node> getParent() const override;

  std::shared_ptr<Node> getThis() override;

# pragma mark -
# pragma mark Accessors

  string getName() const override;

  Node & setName(std::string) override;

  std::shared_ptr<Address> getAddress() const override;

# pragma mark -
# pragma mark Address

  std::shared_ptr<Address> createAddress(Type type) override;

  bool removeAddress() override;

# pragma mark -
# pragma mark Children

  Container<Node>::iterator emplace(Container<Node>::const_iterator pos, string name) override;

  Container<Node>::iterator emplace(Container<Node>::const_iterator pos,
                                    const string& name,
                                    Type type,
                                    AccessMode,
                                    const std::shared_ptr<Domain>&,
                                    BoundingMode,
                                    bool repetitionFilter) override;

  Container<Node>::iterator insert(Container<Node>::const_iterator, std::shared_ptr<Node>, std::string) override;

  Container<Node>::iterator erase(Container<Node>::const_iterator) override;

# pragma mark -
# pragma mark Callback Container

  Node::iterator addCallback(NodeChangeCallback) override;

  void removeCallback(Node::iterator) override;

# pragma mark -
# pragma mark Implementation specific

  //! \note don't put implementation specifics stuff in private or protected as they are invisible for API users
  //! \note this allow to avoid friendship for each Protocol for example

  TTObject & getApplication() const;

  TTSymbol getApplicationType() const;

  /* start namespace update from this node */
  bool updateChildren();

  /* recursive method to build children from the Jamoma tree */
  void buildChildren();

  /* build the address depending on the Jamoma node object */
  void buildAddress();

  /* get any child change back */
  void childNodeChangeCallback(const Node&, const std::string&, NodeChange);

  /* remove all Addresses by closing the listening before deletion */
  void removeAddresses();
};


