#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <iostream>
namespace ossia
{
namespace net
{
std::string sanitize_name(std::string ret)
{
    for(auto& c : ret)
    {
        if(is_valid_character_for_name(c))
            continue;
        else
            c = '_';
    }
    return ret;
}

std::string sanitize_name(std::string name_base, const std::vector<std::string>& brethren)
{
    auto name = sanitize_name(name_base);
    auto len = name.length();
    bool is_here = false;
    std::vector<int> instance_num;
    instance_num.reserve(brethren.size());

    for (const std::string& n_name : brethren)
    {
      if (n_name == name)
      {
        is_here = true;
      }
      else
      {
        if (n_name.size() <= len)
          continue;

        if (n_name.compare(0, len, name) == 0 && n_name[len] == '.')
        {
          // Instance
          try
          {
            int n = std::stoi(n_name.substr(len)); // OPTIMIZEME
            instance_num.push_back(n);
          }
          catch (...)
          {
            continue;
          }
        }
      }
    };

    if (is_here)
    {
      int n = instance_num.size();
      if (n == 0)
      {
        name += ".1";
      }
      else
      {
        // Find first number not in list
        std::sort(instance_num.begin(), instance_num.end());
        int i = 0;

        while (true)
        {
          if (i < n)
          {
            if ((1 + i) == instance_num[i])
            {
              i++;
            }
            else
            {
              name += "." + std::to_string(1 + i);
              break;
            }
          }
          else
          {
            name += "." + std::to_string(1 + i);
            break;
          }
        }
      }
    }
    return name;
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
