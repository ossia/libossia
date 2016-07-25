/*!
 * \file JamomaAddress.h
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
#include "BasicNode.h"
#include "BasicDevice.h"
#include <Editor/Value/Value.h>

#include <thread>
#include <mutex>

namespace OSSIA
{
class Node2;
class Value;
/*! to get the value back
 \param the returned value */
using ValueCallback = std::function<void(const Value&)>;

class OSSIA_EXPORT Address2 : public CallbackContainer<ValueCallback>
{

public:
  using iterator = typename CallbackContainer<ValueCallback>::iterator;
  virtual ~Address2();

  virtual const std::shared_ptr<Node> getNode() const = 0;

  virtual void pullValue() = 0;
  virtual Address2 & pushValue(const Value&) = 0;
  virtual Address2 & pushValue() = 0;

  virtual Value cloneValue(std::vector<char> = {}) const = 0;
  virtual Address2 & setValue(const Value&) = 0;

  virtual Type getValueType() const = 0;
  virtual Address2 & setValueType(Type) = 0;

  virtual AccessMode getAccessMode() const = 0;
  virtual Address2 & setAccessMode(AccessMode) = 0;

  virtual const std::shared_ptr<Domain> & getDomain() const = 0;
  virtual Address2 & setDomain(std::shared_ptr<Domain>) = 0;

  virtual BoundingMode getBoundingMode() const = 0;
  virtual Address2 & setBoundingMode(BoundingMode) = 0;

  virtual bool getRepetitionFilter() const = 0;
  virtual Address2 & setRepetitionFilter(bool = true) = 0;
};

/*!
 * \brief getAddressFromNode
 * \return the textual address of a node : aDevice:/an/address
 */
OSSIA_EXPORT std::string getAddressFromNode(const OSSIA::Node&);
}

namespace impl
{
class BasicAddress final : public Address2
{
private:
  weak_ptr<Node>      mNode;
  mutable weak_ptr<OSSIA::Protocol>  mProtocolCache;

  Value               mValue;
  mutable std::mutex  mValueMutex;

  Type                mValueType;
  AccessMode          mAccessMode;
  BoundingMode        mBoundingMode;
  bool                mRepetitionFilter;

  shared_ptr<Domain>  mDomain;

  ValueCallback       mCallback;
  std::string         mTextualAddress;

public:
  BasicAddress(weak_ptr<Node> node);

  ~BasicAddress();

  const shared_ptr<Node> getNode() const override;

  void pullValue() override;

  OSSIA::Address2 & pushValue(const Value&) override;
  OSSIA::Address2 & pushValue() override;

  const Value& getValue() const;
  Value cloneValue(std::vector<char> = {}) const override;
  Address2 & setValue(const Value&) override;

  Type getValueType() const override;
  OSSIA::Address2 & setValueType(Type) override;

  AccessMode getAccessMode() const override;
  OSSIA::Address2 & setAccessMode(AccessMode) override;

  const shared_ptr<Domain> & getDomain() const override;
  OSSIA::Address2 & setDomain(shared_ptr<Domain>) override;

  BoundingMode getBoundingMode() const override;
  OSSIA::Address2 & setBoundingMode(BoundingMode) override;

  bool getRepetitionFilter() const override;
  OSSIA::Address2 & setRepetitionFilter(bool) override;

  OSSIA::Address2::iterator addCallback(ValueCallback) override;
  void removeCallback(Address::iterator) override;

  static string buildNodePath(std::shared_ptr<Node>);

  Protocol& getProtocol() const;
  const std::string& getTextualAddress() const
  { return mTextualAddress; }

private:
};


OSSIA::Value initValue(OSSIA::Type);
Protocol& getDummyProtocol();

}
