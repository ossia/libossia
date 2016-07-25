#pragma once
#include "Node.hpp"

#include <ossia_export.h>


namespace OSSIA
{
namespace v2
{
class Protocol2;

class OSSIA_EXPORT Device2 : public virtual Node2
{

public:
  Device2() = default;
  Device2(const Device2&) = delete;
  Device2(Device2&&) = delete;
  Device2& operator=(const Device2&) = delete;
  Device2& operator=(Device2&&) = delete;

  virtual ~Device2();

  virtual OSSIA::v2::Protocol2& getProtocol() const = 0;
  virtual bool updateNamespace() = 0;
};
}
}
