#pragma once
#include "Node.hpp"

#include <ossia_export.h>


namespace OSSIA
{
namespace v2
{
class Protocol;

class OSSIA_EXPORT Device : public virtual OSSIA::v2::Node
{
    public:
        Device() = default;
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        virtual ~Device();

        virtual OSSIA::v2::Protocol& getProtocol() const = 0;
        virtual bool updateNamespace() = 0;

        Nano::Signal<void(const Node&)> onNodeCreated; // The node being created
        Nano::Signal<void(const Node&)> onNodeRemoving; // The node being removed
        Nano::Signal<void(const Node&, std::string)> onNodeRenamed; // Node has the new name, second argument is the old name
        Nano::Signal<void(const Address&)> onAddressCreated; // The address being created
        Nano::Signal<void(const Address&)> onAddressRemoving; // The node whose address was removed
};
}
}
