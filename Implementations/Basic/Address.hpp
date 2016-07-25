#pragma once
#include <Editor/Domain.h>
#include <Misc/CallbackContainer.h>
#include <Misc/DestinationIndex.h>
#include <Network/AddressProperties.h>

#include <ossia_export.h>

#include <functional>
#include <memory>

namespace OSSIA
{
class Value;
namespace v2
{
class Node2;

using ValueCallback = std::function<void(const Value&)>;

class OSSIA_EXPORT Address2 : public CallbackContainer<ValueCallback>
{

public:
        Address2() = default;
        Address2(const Address2&) = delete;
        Address2(Address2&&) = delete;
        Address2& operator=(const Address2&) = delete;
        Address2& operator=(Address2&&) = delete;
  using iterator = typename CallbackContainer<ValueCallback>::iterator;
  virtual ~Address2();

  virtual const OSSIA::v2::Node2& getNode() const = 0;

  virtual void pullValue() = 0;
  virtual Address2 & pushValue(const Value&) = 0;
  virtual Address2 & pushValue() = 0;

  virtual Value cloneValue(DestinationIndex = {}) const = 0;
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
OSSIA_EXPORT std::string getAddressFromNode2(const OSSIA::v2::Node2&);
}
}
