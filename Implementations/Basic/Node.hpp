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
class Device2;
class Address2;
class Node2;
using NodeChangeCallback = std::function<void(const Node2&, const std::string&, NodeChange)>;

class OSSIA_EXPORT Node2 : public CallbackContainer<NodeChangeCallback>
{

    public:

        Node2() = default;
        Node2(const Node2&) = delete;
        Node2(Node2&&) = delete;
        Node2& operator=(const Node2&) = delete;
        Node2& operator=(Node2&&) = delete;
        using iterator = typename CallbackContainer<NodeChangeCallback>::iterator;

        using container_iterator = Container<Node2>::iterator;
        using container_const_iterator = Container<Node2>::const_iterator;
        virtual ~Node2();

        virtual std::shared_ptr<OSSIA::v2::Device2> getDevice() const = 0;
        virtual std::shared_ptr<OSSIA::v2::Node2> getParent() const = 0;
        virtual std::shared_ptr<OSSIA::v2::Node2> getThis() = 0;

        virtual std::string getName() const = 0;
        virtual OSSIA::v2::Node2 & setName(std::string) = 0;
        virtual OSSIA::v2::Address2* getAddress() const = 0;

        virtual OSSIA::v2::Address2* createAddress(Type = Type::IMPULSE) = 0;

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
                                          std::shared_ptr<Node2>,
                                          std::string) = 0;

        virtual container_iterator erase(container_const_iterator) = 0;

        const Container<Node2>& children() const
        { return m_children; }

    protected:
        Container<Node2> m_children;
};

}
}
