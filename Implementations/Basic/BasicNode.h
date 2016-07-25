#pragma once
#include "Node.hpp"
#include <Editor/Domain.h>
#include <Network/AddressProperties.h>
#include <Misc/CallbackContainer.h>
#include "Misc/JamomaContainer.h"
#include <functional>
#include <map>
namespace OSSIA
{
namespace v2
{
class Protocol;
}
}
namespace impl
{
class BasicNode :
        public virtual OSSIA::v2::Node,
        public std::enable_shared_from_this<BasicNode>
{

    protected:
        std::string             mName;
        std::weak_ptr<OSSIA::v2::Device>        mDevice;
        std::weak_ptr<BasicNode>    mParent;
        std::unique_ptr<OSSIA::v2::Address>     mAddress;

        bool                    mIsDevice{false};

        std::map<std::shared_ptr<OSSIA::v2::Node>, Node::iterator> mChildNodeChangeCallbackIndexes;

    public:

        BasicNode(
                std::shared_ptr<OSSIA::v2::Device> aDevice = nullptr,
                shared_ptr<BasicNode> aParent = nullptr);

        ~BasicNode();

        std::shared_ptr<OSSIA::v2::Device> getDevice() const override;
        std::shared_ptr<OSSIA::v2::Node> getParent() const override;
        std::shared_ptr<OSSIA::v2::Node> getThis() override;

        std::string getName() const override;
        OSSIA::v2::Node& setName(std::string) override;

        OSSIA::v2::Address* getAddress() const override;
        OSSIA::v2::Address* createAddress(OSSIA::Type type) override;
        bool removeAddress() override;


        container_iterator emplace(container_const_iterator pos, std::string name) override;
        container_iterator emplace(container_const_iterator pos,
                                   const std::string& name,
                                   OSSIA::Type type,
                                   OSSIA::AccessMode,
                                   const std::shared_ptr<OSSIA::Domain>&,
                                   OSSIA::BoundingMode,
                                   bool repetitionFilter) override;

        container_iterator insert(container_const_iterator, std::shared_ptr<OSSIA::v2::Node>, std::string) override;
        container_iterator erase(container_const_iterator) override;

        Node::iterator addCallback(OSSIA::v2::NodeChangeCallback) override;

        void removeCallback(Node::iterator) override;
};
}
