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

#include "BasicProtocol.h"
#include "BasicAddress.h"

#include "Misc/JamomaContainer.h"
#include <functional>
#include <map>
namespace OSSIA
{

enum class NodeChange
{
  RENAMED,
  EMPLACED,
  ERASED,
  ADDRESS_CREATED,
  ADDRESS_REMOVED
};
using NodeChangeCallback2 = std::function<void(const Node2&, const std::string&, NodeChange)>;

class OSSIA_EXPORT Node2 : public CallbackContainer<NodeChangeCallback>
{

public:

  using iterator = typename CallbackContainer<NodeChangeCallback2>::iterator;

  using container_iterator = Container<Node2>::iterator;
  using container_const_iterator = Container<Node2>::const_iterator;
  virtual ~Node2();

  virtual std::shared_ptr<Device> getDevice() const = 0;
  virtual std::shared_ptr<Node2> getParent() const = 0;
  virtual std::shared_ptr<Node2> getThis() = 0;

  virtual std::string getName() const = 0;
  virtual Node2 & setName(std::string) = 0;
  virtual std::shared_ptr<OSSIA::Address> getAddress() const = 0;

  virtual std::shared_ptr<OSSIA::Address> createAddress(Type = Type::IMPULSE) = 0;

  virtual bool removeAddress() = 0;

  virtual container_iterator emplace(container_const_iterator, std::string) = 0;

  virtual container_iterator emplace(container_const_iterator,
                                            const std::string&,
                                            Type,
                                            AccessMode = {},
                                            const std::shared_ptr<Domain>& = {},
                                            BoundingMode = {},
                                            bool repetitionFilter = {}) = 0;

  virtual container_iterator insert(container_const_iterator,
                                           std::shared_ptr<Node>,
                                           std::string) = 0;

  virtual container_iterator erase(container_const_iterator) = 0;

  const Container<Node>& children() const
  { return m_children; }

protected:
  Container<Node> m_children;
};

}
namespace impl
{
class BasicNode :
    public virtual Node2,
    public enable_shared_from_this<BasicNode>
{

protected:
  std::string             mName;
  weak_ptr<Device2>        mDevice;
  weak_ptr<BasicNode>    mParent;
  unique_ptr<Address>     mAddress;

  bool                    mIsDevice{false};

  map<shared_ptr<Node2>, Node2::iterator> mChildNodeChangeCallbackIndexes;

public:

  BasicNode(shared_ptr<Device2> aDevice = nullptr, shared_ptr<BasicNode> aParent = nullptr);

  ~BasicNode();

  shared_ptr<Device2> getDevice() const override;
  shared_ptr<Node2> getParent() const override;
  shared_ptr<Node2> getThis() override;

  string getName() const override;
  Node & setName(std::string) override;

  Address2* getAddress() const override;
  Address2* createAddress(Type type) override;
  bool removeAddress() override;


  container_iterator emplace(container_const_iterator pos, string name) override;
  container_iterator emplace(container_const_iterator pos,
                                    const string& name,
                                    Type type,
                                    AccessMode,
                                    const std::shared_ptr<Domain>&,
                                    BoundingMode,
                                    bool repetitionFilter) override;

  container_iterator insert(container_const_iterator, shared_ptr<Node>, std::string) override;
  container_iterator erase(container_const_iterator) override;

  Node2::iterator addCallback(NodeChangeCallback) override;

  void removeCallback(Node2::iterator) override;
};
}
