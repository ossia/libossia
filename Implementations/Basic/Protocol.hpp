#pragma once
#include <memory>
#include <ossia_export.h>

namespace OSSIA
{
class NetworkLogger;

namespace v2
{
class Address2;
class Node2;
class Device2;
class OSSIA_EXPORT Protocol2
{
public:
  virtual ~Protocol2();

  virtual bool pullAddressValue(Address2&) const = 0;
  virtual bool pushAddressValue(const Address2&) const = 0;

  virtual bool observeAddressValue(Address2&, bool) const = 0;

  virtual bool updateChildren(Node2& node) const = 0;

  virtual void setLogger(std::shared_ptr<NetworkLogger>) = 0;
  virtual std::shared_ptr<NetworkLogger> getLogger() const = 0;
};
}
}
