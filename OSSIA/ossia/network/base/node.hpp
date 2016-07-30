#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/network/common/address_properties.hpp>

#include <string>
#include <functional>
#include <memory>

#include <ossia_export.h>
#include <nano_signal_slot.hpp>

namespace ossia
{
namespace net
{
class device;
class address;
class node;

class OSSIA_EXPORT node
{
    public:
        node() = default;
        node(const node&) = delete;
        node(node&&) = delete;
        node& operator=(const node&) = delete;
        node& operator=(node&&) = delete;

        virtual ~node();

        virtual device& getDevice() const = 0;
        virtual node* getParent() const = 0;

        virtual std::string getName() const = 0;
        virtual node & setName(std::string) = 0;

        virtual address* getAddress() const = 0;
        virtual address* createAddress(Type = Type::IMPULSE) = 0;
        virtual bool removeAddress() = 0;

        // The parent has ownership
        node* createChild(const std::string& name);
        bool removeChild(const std::string& name);
        bool removeChild(const node& name);
        void clearChildren();

        const std::vector<std::unique_ptr<node>>& children() const
        { return mChildren; }

    protected:
        virtual std::unique_ptr<node> makeChild(const std::string& name) = 0;
        virtual void removingChild(node& node) = 0;

        std::vector<std::unique_ptr<node>> mChildren;
};

}
}
