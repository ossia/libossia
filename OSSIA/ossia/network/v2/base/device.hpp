#pragma once
#include <ossia/network/v2/base/node.hpp>

#include <ossia_export.h>


namespace OSSIA
{
namespace v2
{
class Protocol;

class OSSIA_EXPORT Device :
        public virtual OSSIA::v2::Node
{
    public:
        Device(std::unique_ptr<OSSIA::v2::Protocol> proto);

        Device() = delete;
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        OSSIA::v2::Protocol& getProtocol() const;

        virtual ~Device();

        virtual bool updateNamespace() { return false; }

        Nano::Signal<void(const Node&)> onNodeCreated; // The node being created
        Nano::Signal<void(const Node&)> onNodeRemoving; // The node being removed
        Nano::Signal<void(const Node&, std::string)> onNodeRenamed; // Node has the new name, second argument is the old name
        Nano::Signal<void(const Address&)> onAddressCreated; // The address being created
        Nano::Signal<void(const Address&)> onAddressRemoving; // The node whose address was removed

    protected:
        std::unique_ptr<OSSIA::v2::Protocol> mProtocol;
};
}
}
