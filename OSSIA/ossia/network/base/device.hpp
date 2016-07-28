#pragma once
#include <ossia/network/base/node.hpp>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace OSSIA
{
namespace net
{
class Protocol;

class OSSIA_EXPORT Device
{
    public:
        Device(std::unique_ptr<OSSIA::net::Protocol> proto);

        Device() = delete;
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        OSSIA::net::Protocol& getProtocol() const;

        virtual ~Device();

        virtual const OSSIA::net::Node& getRootNode() const = 0;
        virtual OSSIA::net::Node& getRootNode() = 0;

        void setName(const std::string& str)
        { getRootNode().setName(str); }
        std::string getName() const
        { return getRootNode().getName(); }

        virtual bool updateNamespace() { return false; }

        Nano::Signal<void(const Node&)> onNodeCreated; // The node being created
        Nano::Signal<void(const Node&)> onNodeRemoving; // The node being removed
        Nano::Signal<void(const Node&, std::string)> onNodeRenamed; // Node has the new name, second argument is the old name
        Nano::Signal<void(const Address&)> onAddressCreated; // The address being created
        Nano::Signal<void(const Address&)> onAddressRemoving; // The node whose address was removed

    protected:
        std::unique_ptr<OSSIA::net::Protocol> mProtocol;
};
}
}
