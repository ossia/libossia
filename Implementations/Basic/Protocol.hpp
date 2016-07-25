#pragma once
#include <memory>
#include <ossia_export.h>

namespace OSSIA
{
class NetworkLogger;

namespace v2
{
class Address;
class Node;
class Device;
class OSSIA_EXPORT Protocol
{
    public:
        virtual ~Protocol();

        virtual bool pullAddressValue(Address&) const = 0;
        virtual bool pushAddressValue(const Address&) const = 0;

        virtual bool observeAddressValue(Address&, bool) const = 0;

        virtual bool updateChildren(Node& node) const = 0;

        void setLogger(const std::shared_ptr<NetworkLogger>& l) { mLogger = l; }
        const std::shared_ptr<NetworkLogger>& getLogger() const { return mLogger; }

    protected:
        std::shared_ptr<OSSIA::NetworkLogger> mLogger;
};
}
}
