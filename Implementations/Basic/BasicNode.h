#pragma once
#include "Node.hpp"
#include "BasicProtocol.h"
#include <Editor/Domain.h>
#include <Network/AddressProperties.h>
#include <Misc/CallbackContainer.h>
#include "Misc/JamomaContainer.h"
#include <functional>
#include <map>
namespace impl
{
class BasicNode :
        public virtual OSSIA::v2::Node2,
        public std::enable_shared_from_this<BasicNode>
{

    protected:
        std::string             mName;
        std::weak_ptr<OSSIA::v2::Device2>        mDevice;
        std::weak_ptr<BasicNode>    mParent;
        std::unique_ptr<OSSIA::v2::Address2>     mAddress;

        bool                    mIsDevice{false};

        std::map<std::shared_ptr<OSSIA::v2::Node2>, Node2::iterator> mChildNodeChangeCallbackIndexes;

    public:

        BasicNode(
                std::shared_ptr<OSSIA::v2::Device2> aDevice = nullptr,
                shared_ptr<BasicNode> aParent = nullptr);

        ~BasicNode();

        std::shared_ptr<OSSIA::v2::Device2> getDevice() const override;
        std::shared_ptr<OSSIA::v2::Node2> getParent() const override;
        std::shared_ptr<OSSIA::v2::Node2> getThis() override;

        std::string getName() const override;
        OSSIA::v2::Node2& setName(std::string) override;

        OSSIA::v2::Address2* getAddress() const override;
        OSSIA::v2::Address2* createAddress(OSSIA::Type type) override;
        bool removeAddress() override;


        container_iterator emplace(container_const_iterator pos, std::string name) override;
        container_iterator emplace(container_const_iterator pos,
                                   const std::string& name,
                                   OSSIA::Type type,
                                   OSSIA::AccessMode,
                                   const std::shared_ptr<OSSIA::Domain>&,
                                   OSSIA::BoundingMode,
                                   bool repetitionFilter) override;

        container_iterator insert(container_const_iterator, std::shared_ptr<OSSIA::v2::Node2>, std::string) override;
        container_iterator erase(container_const_iterator) override;

        Node2::iterator addCallback(OSSIA::v2::NodeChangeCallback) override;

        void removeCallback(Node2::iterator) override;
};
}
