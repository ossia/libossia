#pragma once
#include <memory>
#include <ossia_export.h>

namespace ossia
{
class NetworkLogger;
class State;

namespace net
{
class address;
class Node;
class Device;
class OSSIA_EXPORT Protocol
{
  public:
    virtual ~Protocol();

    virtual bool pull(address&) = 0;
    virtual bool push(const address&) = 0;
    virtual bool push(const State&) { return false; }

    virtual bool observe(address&, bool) = 0;

    virtual bool update(Node& node) = 0;

    virtual void setDevice(ossia::net::Device& dev){ }

    void setLogger(const std::shared_ptr<NetworkLogger>& l) { mLogger = l; }
    const std::shared_ptr<NetworkLogger>& getLogger() const { return mLogger; }

  protected:
    std::shared_ptr<ossia::NetworkLogger> mLogger;
};
}
}
