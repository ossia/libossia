#pragma once
#include <Misc/CallbackContainer.h>
#include <Misc/Container.h>
#include <Editor/Domain.h>
#include <Network/AddressProperties.h>

#include <string>
#include <functional>
#include <memory>

#include <ossia_export.h>

namespace OSSIA
{
enum class NodeChange
{
    RENAMED,
    EMPLACED,
    ERASED,
    ADDRESS_CREATED,
    ADDRESS_REMOVED
};

namespace v2
{
class Device;
class Address;
class Node;
using NodeChangeCallback = std::function<void(const Node&, const std::string&, NodeChange)>;

class OSSIA_EXPORT Node : public CallbackContainer<NodeChangeCallback>
{

    public:

        Node() = default;
        Node(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;
        using iterator = typename CallbackContainer<NodeChangeCallback>::iterator;

        using container_iterator = Container<Node>::iterator;
        using container_const_iterator = Container<Node>::const_iterator;
        virtual ~Node();

        virtual std::shared_ptr<OSSIA::v2::Device> getDevice() const = 0;
        virtual std::shared_ptr<OSSIA::v2::Node> getParent() const = 0;
        virtual std::shared_ptr<OSSIA::v2::Node> getThis() = 0;

        virtual std::string getName() const = 0;
        virtual OSSIA::v2::Node & setName(std::string) = 0;
        virtual OSSIA::v2::Address* getAddress() const = 0;

        virtual OSSIA::v2::Address* createAddress(Type = Type::IMPULSE) = 0;

        virtual bool removeAddress() = 0;

        virtual container_iterator emplace(container_const_iterator, std::string) = 0;

        virtual container_iterator emplace(container_const_iterator,
                                           const std::string&,
                                           Type,
                                           AccessMode = {},
                                           const std::shared_ptr<OSSIA::Domain>& = {},
                                           BoundingMode = {},
                                           bool repetitionFilter = {}) = 0;

        virtual container_iterator insert(container_const_iterator,
                                          std::shared_ptr<Node>,
                                          std::string) = 0;

        virtual container_iterator erase(container_const_iterator) = 0;

        const Container<Node>& children() const
        { return m_children; }

    protected:
        Container<Node> m_children;
};

}
}
