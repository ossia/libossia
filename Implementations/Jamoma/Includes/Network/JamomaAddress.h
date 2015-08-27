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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"

#include "Network/JamomaProtocol.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaProtocol;

class JamomaAddress : public Address
{

  friend JamomaProtocol;
  
private:

# pragma mark -
# pragma mark Implementation specific
  
  weak_ptr<Node>      mNode;
  
  mutable TTObject    mObject;
  mutable Value *     mValue{};
  Value::Type         mValueType;
  AccessMode          mAccessMode;
  BoundingMode        mBoundingMode;
  bool                mRepetitionFilter;
  
  shared_ptr<Domain>  mDomain;
  
  ValueCallback       mCallback;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaAddress(shared_ptr<Node> node, TTObject aData = TTObject());
  
  ~JamomaAddress();
  
# pragma mark -
# pragma mark Network
  
  const shared_ptr<Node> getNode() const override;
  
  const Value * pullValue() override;
  
  Address & pushValue(const Value * = nullptr) override;
  
# pragma mark -
# pragma mark Accessors

  const Value * getValue() const override;

  Address & setValue(const Value *) override;

  Value::Type getValueType() const override;

  AccessMode getAccessMode() const override;

  Address & setAccessMode(AccessMode) override;
  
  const shared_ptr<Domain> & getDomain() const override;
  
  Address & setDomain(shared_ptr<Domain>) override;
  
  BoundingMode getBoundingMode() const override;

  Address & setBoundingMode(BoundingMode) override;

  bool getRepetitionFilter() const override;

  Address & setRepetitionFilter(bool) override;

# pragma mark -
# pragma mark Expression

  bool evaluate() const override;
 
# pragma mark -
# pragma mark Callback Container
  
  CallbackContainer<ValueCallback>::CallbackIterator addCallback(ValueCallback) override;
  
  void removeCallback(typename CallbackContainer<ValueCallback>::CallbackIterator) override;

# pragma mark -
# pragma mark Implementation specific

private:

  static TTErr TTValueCallback(const TTValue&, const TTValue&);

  Value * convertTTValueIntoValue(const TTValue&, Value::Type) const;
  
  void convertValueIntoTTValue(const Value *, TTValue &) const;
  
  string buildNodePath(shared_ptr<Node>) const;
};
