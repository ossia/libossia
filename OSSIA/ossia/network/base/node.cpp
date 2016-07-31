#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
namespace net
{
node_base::~node_base() = default;

node_base* node_base::createChild(const std::string& name)
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

bool node_base::removeChild(const std::string& name)
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

bool node_base::removeChild(const node_base& n)
{
    auto it = find_if(mChildren, [&] (const auto& c) {
        return c.get() == &n;
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

void node_base::clearChildren()
{
    for(auto& child : mChildren)
        removingChild(*child);
    mChildren.clear();
}

}
}
