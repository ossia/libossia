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
#include "Network/Node.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaAddress : public Address
{

private:

# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Device>  mDevice;
  
  mutable TTObject    mData;
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

  JamomaAddress(shared_ptr<Device> device, TTObject aData = TTObject());
  
  ~JamomaAddress();

# pragma mark -
# pragma mark Value

  bool updateValue() const override;

  const Value * getValue() const override;

  bool sendValue(const Value * value) const override;

# pragma mark -
# pragma mark Network

  const shared_ptr<Device> & getDevice() const override;

# pragma mark -
# pragma mark Accessors

  Value::Type getValueType() const override;

  AccessMode getAccessMode() const override;

  Address & setAccessMode(AccessMode accessMode) override;
  
  const shared_ptr<Domain> & getDomain() const override;
  
  Address & setDomain(shared_ptr<Domain> domain) override;
  
  BoundingMode getBoundingMode() const override;

  Address & setBoundingMode(BoundingMode boundingMode) override;

  bool getRepetitionFilter() const override;

  Address & setRepetitionFilter(bool repetitionFilter) override;

# pragma mark -
# pragma mark Callback

  Address::ValueCallback getValueCallback() const override;
 
  void setValueCallback(Address::ValueCallback callback) override;

# pragma mark -
# pragma mark Expression

  bool evaluate() const override;

# pragma mark -
# pragma mark Implementation specific

private:

  static TTErr ValueCallback(const TTValue& baton, const TTValue& value);

  Value * convertTTValueIntoValue(const TTValue& v, Value::Type valueType) const;
  
  void convertValueIntoTTValue(const Value * value, TTValue & v) const;
  
  string buildNodePath(shared_ptr<Node> node) const;
};
