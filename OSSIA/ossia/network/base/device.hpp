#pragma once
#include <ossia/network/base/node.hpp>
#include <nano_signal_slot.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class protocol;

class OSSIA_EXPORT device
{
    public:
        device(std::unique_ptr<ossia::net::protocol> proto);

        device() = delete;
        device(const device&) = delete;
        device(device&&) = delete;
        device& operator=(const device&) = delete;
        device& operator=(device&&) = delete;

        ossia::net::protocol& getProtocol() const;

        virtual ~device();

        virtual const ossia::net::node& getRootNode() const = 0;
        virtual ossia::net::node& getRootNode() = 0;

        void setName(const std::string& str)
        { getRootNode().setName(str); }
        std::string getName() const
        { return getRootNode().getName(); }

        virtual bool updateNamespace() { return false; }

        Nano::Signal<void(const node&)> onNodeCreated; // The node being created
        Nano::Signal<void(const node&)> onNodeRemoving; // The node being removed
        Nano::Signal<void(const node&, std::string)> onNodeRenamed; // Node has the new name, second argument is the old name
        Nano::Signal<void(const address&)> onAddressCreated; // The address being created
        Nano::Signal<void(const address&)> onAddressRemoving; // The node whose address was removed

    protected:
        std::unique_ptr<ossia::net::protocol> mProtocol;
};
}
}
