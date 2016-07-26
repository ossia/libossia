#pragma once
#include "Node.hpp"
#include <Network/AddressProperties.h>
#include <Misc/CallbackContainer.h>
#include "Misc/JamomaContainer.hpp"
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
        public virtual OSSIA::v2::Node
{

    protected:
        std::string mName;
        OSSIA::v2::Device& mDevice;
        BasicNode* mParent{};
        std::unique_ptr<OSSIA::v2::Address> mAddress;

    public:
        BasicNode() = delete;
        BasicNode(const BasicNode&) = delete;
        BasicNode(BasicNode&&) = delete;
        BasicNode& operator=(const BasicNode&) = delete;
        BasicNode& operator=(BasicNode&&) = delete;

        BasicNode(
                std::string name,
                OSSIA::v2::Device& aDevice,
                BasicNode& aParent);

        BasicNode(std::string name,
                  OSSIA::v2::Device& aDevice);

        ~BasicNode();

        OSSIA::v2::Device& getDevice() const final override { return mDevice; }
        OSSIA::v2::Node* getParent() const final override { return mParent; }

        std::string getName() const override { return mName; }
        OSSIA::v2::Node& setName(std::string) override;

        OSSIA::v2::Address* getAddress() const override;
        OSSIA::v2::Address* createAddress(OSSIA::Type type) override;
        bool removeAddress() override;

    private:
        std::unique_ptr<Node> makeChild(const std::string& name) override
        {
            return std::make_unique<BasicNode>(name, mDevice, *this);
        }

        void removingChild(Node& node) override
        {

        }
};

}
