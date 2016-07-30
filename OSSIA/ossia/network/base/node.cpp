#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
namespace net
{
node::~node() = default;

node* node::createChild(const std::string& name)
{
    auto res = makeChild(name);

    auto ptr = res.get();
    if(res)
    {
        mChildren.push_back(std::move(res));
        getDevice().onNodeCreated(*ptr);
    }

    return ptr;
}

bool node::removeChild(const std::string& name)
{
    auto it = find_if(mChildren, [&] (const auto& c) {
        return c->getName() == name;
    });

    if(it != mChildren.end())
    {
        getDevice().onNodeRemoving(**it);
        removingChild(**it);
        mChildren.erase(it);

        return true;
    }
    else
    {
        return false;
    }
}

bool node::removeChild(const node& node)
{
    auto it = find_if(mChildren, [&] (const auto& c) {
        return c.get() == &node;
    });

    if(it != mChildren.end())
    {
        getDevice().onNodeRemoving(**it);
        removingChild(**it);
        mChildren.erase(it);

        return true;
    }
    else
    {
        return false;
    }
}

void node::clearChildren()
{
    for(auto& child : mChildren)
        removingChild(*child);
    mChildren.clear();
}

}
}
