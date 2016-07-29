#pragma once
#include <ossia/network/base/node.hpp>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class Protocol;

class OSSIA_EXPORT Device
{
    public:
        Device(std::unique_ptr<ossia::net::Protocol> proto);

        Device() = delete;
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        ossia::net::Protocol& getProtocol() const;

        virtual ~Device();

        virtual const ossia::net::Node& getRootNode() const = 0;
        virtual ossia::net::Node& getRootNode() = 0;

        void setName(const std::string& str)
        { getRootNode().setName(str); }
        std::string getName() const
        { return getRootNode().getName(); }

        virtual bool updateNamespace() { return false; }

        Nano::Signal<void(const Node&)> onNodeCreated; // The node being created
        Nano::Signal<void(const Node&)> onNodeRemoving; // The node being removed
        Nano::Signal<void(const Node&, std::string)> onNodeRenamed; // Node has the new name, second argument is the old name
        Nano::Signal<void(const address&)> onAddressCreated; // The address being created
        Nano::Signal<void(const address&)> onAddressRemoving; // The node whose address was removed

    protected:
        std::unique_ptr<ossia::net::Protocol> mProtocol;
};
}
}
