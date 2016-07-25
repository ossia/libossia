#pragma once
#include <Misc/CallbackContainer.h>
#include <Misc/Container.h>
#include <Editor/Domain.h>
#include <Network/AddressProperties.h>

#include <string>
#include <functional>
#include <memory>

#include <ossia_export.h>
#include <nano_signal_slot.hpp>

namespace OSSIA
{
namespace v2
{
class Device;
class Address;
class Node;

class OSSIA_EXPORT Node
{
    public:
        Node() = default;
        Node(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;

        virtual ~Node();

        virtual Device& getDevice() const = 0;
        virtual Node* getParent() const = 0;

        virtual std::string getName() const = 0;
        virtual Node & setName(std::string) = 0;

        virtual Address* getAddress() const = 0;
        virtual Address* createAddress(Type = Type::IMPULSE) = 0;
        virtual bool removeAddress() = 0;

        // The parent has ownership
        Node* createChild(const std::string& name);
        bool removeChild(const std::string& name);
        bool removeChild(const Node& name);

        const std::vector<std::unique_ptr<Node>>& children() const
        { return mChildren; }

    protected:
        virtual std::unique_ptr<Node> makeChild(const std::string& name) = 0;
        virtual void removingChild(Node& node) = 0;

        std::vector<std::unique_ptr<Node>> mChildren;
};

}
}
