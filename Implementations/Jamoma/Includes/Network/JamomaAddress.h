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
#include "Network/Protocol.h"

#include "TTModular.h"

#include <thread>
#include <mutex>

using namespace OSSIA;
using namespace std;

class JamomaAddress final : public Address, public enable_shared_from_this<JamomaAddress>
{

private:

# pragma mark -
# pragma mark Implementation specific

  weak_ptr<Node>      mNode;

  mutable TTObject    mObject;
  TTObject            mObjectValueCallback;
  mutable Value *     mValue{};
  mutable std::mutex  mValueMutex;

  Value::Type         mValueType;
  AccessMode          mAccessMode;
  BoundingMode        mBoundingMode;
  bool                mRepetitionFilter;

  shared_ptr<Domain>  mDomain;

  ValueCallback       mCallback;
  mutable weak_ptr<OSSIA::Protocol>  mProtocolCache;
  std::string         mTextualAddress;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaAddress(weak_ptr<Node> node, TTObject aData = TTObject());

  ~JamomaAddress();

# pragma mark -
# pragma mark Network

  const shared_ptr<Node> getNode() const override;

  const Value * pullValue() override;

  Address & pushValue(const Value * = nullptr) override;

# pragma mark -
# pragma mark Accessors

  const Value * getValue() const override;

  std::unique_ptr<OSSIA::Value> cloneValue(std::vector<char> = {}) const override;

  Address & setValue(const Value *) override;

  Value::Type getValueType() const override;

  Address & setValueType(Value::Type) override;

  AccessMode getAccessMode() const override;

  Address & setAccessMode(AccessMode) override;

  const shared_ptr<Domain> & getDomain() const override;

  Address & setDomain(shared_ptr<Domain>) override;

  BoundingMode getBoundingMode() const override;

  Address & setBoundingMode(BoundingMode) override;

  bool getRepetitionFilter() const override;

  Address & setRepetitionFilter(bool) override;

# pragma mark -
# pragma mark Callback Container

  Address::iterator addCallback(ValueCallback) override;

  void removeCallback(Address::iterator) override;

# pragma mark -
# pragma mark Implementation specific

  //! \note don't put implementation specifics stuff in private or protected as they are invisible for API users
  //! \note this allow to avoid friendship for each Protocol for example

  /* init value member depending on its type to avoid null pointer */
  void initValue();

  /*! pull TTValue from mObject
   \param TTValue pulled value
   \return true if the operation succeeded */
  bool pullValue(TTValue&) const;

  /*! push TTValue into mObject
   \param TTValue value to push
   \return true if the operation succeeded */
  bool pushValue(const TTValue&) const;

  /*! get TTValue from internal mValue
   \param TTValue returned value */
  void getValue(TTValue&) const;

  /*! set TTValue into internal mValue
   \param TTValue value to set */
  void setValue(const TTValue&);

  /*! register/unregister for mObject value attribute notification
   \param bool true to enable observation */
  void observeValue(bool);

  Value * convertTTValueIntoValue(const TTValue&, Value::Type) const;

  void convertValueIntoTTValue(const Value *, TTValue &) const;

  string buildNodePath(shared_ptr<Node>) const;


  Protocol& getProtocol() const;
  const std::string& getTextualAddress() const
  { return mTextualAddress; }
private:

  static TTErr TTValueCallback(const TTValue&, const TTValue&);
};

/*!
 * \brief getDummyProtocol
 * \return a simple protocol useful when no protocol is available.
 */
Protocol& getDummyProtocol();

/*!
 * \brief getAddressFromNode
 * \return the textual address of a node : aDevice:/an/address
 */
std::string getAddressFromNode(const OSSIA::Node&);

/*!
 * \brief getValueAsString Returns a string corresponding to the value
 * \param val a valid value
 * \return a string in the format : "type: value".
 *
 * ex. "int: 3"
 *     "string: tutu"
 *     "tuple: [ int: 2, float: 3 ]"
 * etc...
 *
 */
std::string getValueAsString(const OSSIA::Value& val);
