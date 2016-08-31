#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <iostream>
namespace ossia
{
namespace net
{
std::string sanitize_name(const std::string& name)
{
    std::string ret = name;

    for(auto& c : ret)
    {
        if((c >= 'a' && c <= 'z')
                || (c >= 'A' && c <= 'Z')
                || (c >= '0' && c <= '9')
                || (c == '.')
                || (c == '~')
                || (c == '_')
                || (c == '(')
                || (c == ')')
                || (c == '-'))
            continue;
        else
            c = '_';
    }
    return ret;
}

node_base::~node_base() = default;

node_base* node_base::createChild(const std::string& name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return nullptr;

  auto res = makeChild(sanitize_name(name));

  auto ptr = res.get();
  if (res)
  {
    mChildren.push_back(std::move(res));
    dev.onNodeCreated(*ptr);
  }

  return ptr;
}

bool node_base::removeChild(const std::string& name)
{
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return false;

  auto it = find_if(
      mChildren, [&](const auto& c) { return c->getName() == sanitize_name(name); });

  if (it != mChildren.end())
  {
    dev.onNodeRemoving(**it);
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
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return false;
  auto it = find_if(mChildren, [&](const auto& c) { return c.get() == &n; });

  if (it != mChildren.end())
  {
    dev.onNodeRemoving(**it);
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
  auto& dev = getDevice();
  if(!dev.getCapabilities().change_tree)
    return;

  for (auto& child : mChildren)
    removingChild(*child);
  mChildren.clear();
}
}
}
