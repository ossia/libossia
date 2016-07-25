#include "Node.hpp"
#include <Misc/Util.h>

namespace OSSIA
{
namespace v2
{
Node::~Node() = default;

Node* Node::createChild(const std::string& name)
{
    auto res = makeChild(name);

    if(res)
        mChildren.push_back(std::move(res));

    return res.get();
}

bool Node::removeChild(const std::string& name)
{
    auto it = find_if(mChildren, [&] (const auto& c) {
        return c->getName() == name;
    });

    if(it != mChildren.end())
    {
        removingChild(**it);
        mChildren.erase(it);

        return true;
    }
    else
    {
        return false;
    }
}

bool Node::removeChild(const Node& node)
{
    auto it = find_if(mChildren, [&] (const auto& c) {
        return c.get() == &node;
    });

    if(it != mChildren.end())
    {
        removingChild(**it);
        mChildren.erase(it);

        return true;
    }
    else
    {
        return false;
    }
}

void Node::clearChildren()
{
    for(auto& child : mChildren)
        removingChild(*child);
    mChildren.clear();
}

}
}
