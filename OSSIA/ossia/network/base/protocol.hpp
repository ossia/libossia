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
class node;
class device;
class OSSIA_EXPORT protocol
{
  public:
    virtual ~protocol();

    virtual bool pull(address&) = 0;
    virtual bool push(const address&) = 0;
    virtual bool pushState(const State&) { return false; }

    virtual bool observe(address&, bool) = 0;

    virtual bool update(node& node) = 0;

    virtual void setDevice(ossia::net::device& dev){ }

    void setLogger(const std::shared_ptr<NetworkLogger>& l) { mLogger = l; }
    const std::shared_ptr<NetworkLogger>& getLogger() const { return mLogger; }

  protected:
    std::shared_ptr<ossia::NetworkLogger> mLogger;
};
}
}
