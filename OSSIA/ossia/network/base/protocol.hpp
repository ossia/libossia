#pragma once
#include <memory>
#include <ossia_export.h>

namespace OSSIA
{
class NetworkLogger;
class State;

namespace net
{
class Address;
class Node;
class Device;
class OSSIA_EXPORT Protocol
{
  public:
    virtual ~Protocol();

    virtual bool pull(Address&) = 0;
    virtual bool push(const Address&) = 0;
    virtual bool push(const State&) { return false; }

    virtual bool observe(Address&, bool) = 0;

    virtual bool update(Node& node) = 0;

    virtual void setDevice(OSSIA::net::Device& dev){ }

    void setLogger(const std::shared_ptr<NetworkLogger>& l) { mLogger = l; }
    const std::shared_ptr<NetworkLogger>& getLogger() const { return mLogger; }

  protected:
    std::shared_ptr<OSSIA::NetworkLogger> mLogger;
};
}
}
