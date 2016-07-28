#pragma once
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/detail/callback_container.hpp>
#include <functional>
#include <map>
namespace OSSIA
{
namespace net
{
class Protocol;
}
}
namespace impl
{
class OSSIA_EXPORT BasicNode :
        public OSSIA::net::Node
{

    protected:
        std::string mName;
        OSSIA::net::Device& mDevice;
        BasicNode* mParent{};
        std::unique_ptr<OSSIA::net::Address> mAddress;

    public:
        BasicNode() = delete;
        BasicNode(const BasicNode&) = delete;
        BasicNode(BasicNode&&) = delete;
        BasicNode& operator=(const BasicNode&) = delete;
        BasicNode& operator=(BasicNode&&) = delete;

        BasicNode(
                std::string name,
                OSSIA::net::Device& aDevice,
                BasicNode& aParent);

        BasicNode(std::string name,
                  OSSIA::net::Device& aDevice);

        ~BasicNode();

        OSSIA::net::Device& getDevice() const final override { return mDevice; }
        OSSIA::net::Node* getParent() const final override { return mParent; }

        std::string getName() const override { return mName; }
        OSSIA::net::Node& setName(std::string) override;

        OSSIA::net::Address* getAddress() const override;
        OSSIA::net::Address* createAddress(OSSIA::Type type) override;
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
