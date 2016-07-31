#pragma once
#include <ossia/network/base/node.hpp>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class protocol_base;

class OSSIA_EXPORT device_base
{
    public:
        device_base(std::unique_ptr<ossia::net::protocol_base> proto);

        device_base() = delete;
        device_base(const device_base&) = delete;
        device_base(device_base&&) = delete;
        device_base& operator=(const device_base&) = delete;
        device_base& operator=(device_base&&) = delete;

        ossia::net::protocol_base& getProtocol() const;

        virtual ~device_base();

        virtual const ossia::net::node_base& getRootNode() const = 0;
        virtual ossia::net::node_base& getRootNode() = 0;

        void setName(const std::string& str)
        { getRootNode().setName(str); }
        std::string getName() const
        { return getRootNode().getName(); }

        virtual bool updateNamespace() { return false; }

        Nano::Signal<void(const node_base&)> onNodeCreated; // The node being created
        Nano::Signal<void(const node_base&)> onNodeRemoving; // The node being removed
        Nano::Signal<void(const node_base&, std::string)> onNodeRenamed; // Node has the new name, second argument is the old name
        Nano::Signal<void(const address_base&)> onAddressCreated; // The address being created
        Nano::Signal<void(const address_base&)> onAddressRemoving; // The node whose address was removed

    protected:
        std::unique_ptr<ossia::net::protocol_base> mProtocol;
};
}
}
