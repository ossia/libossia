#include <ossia/network/common/debug.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <thread>
namespace ossia
{
namespace net
{

fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>& w, const node_base& n)
{
  auto parent = n.getParent();
  while (parent != nullptr)
  {
    w << '\t';
    parent = parent->getParent();
  }
  w << n.getName();
  if(auto addr = n.getAddress())
  {
    w << " : " << value_to_pretty_string(addr->cloneValue())
      << ", AccessMode(" << addr->getAccessMode() << ")"
      << ", BoundingMode(" << addr->getBoundingMode() << ")"
      << ", Domain(" << addr->getDomain() << ")"
      << ", Unit(" << ossia::get_pretty_unit_text(addr->getUnit()) << ")";
  }

  return w;
}

void debug_recursively(fmt::MemoryWriter& w, const node_base& n)
{
  w << n << "\n";
  for(auto& cld: n.children())
    debug_recursively(w, *cld);
}

fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>& w, access_mode a)
{
  switch (a)
  {
    case access_mode::SET :
      w << "set";
      break;
    case access_mode::GET :
      w << "get";
      break;
    case access_mode::BI :
      w << "bi";
      break;
    default:
      break;
  }
  return w;
}

fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>& w, ossia::bounding_mode b)
{
  switch (b)
  {
    case bounding_mode::FREE :
    {
      w << "free";
      break;
    }
    case bounding_mode::CLIP :
    {
      w << "clip";
      break;
    }
    case bounding_mode::WRAP :
    {
      w << "wrap";
      break;
    }
    case bounding_mode::FOLD :
    {
      w << "fold";
      break;
    }
    case bounding_mode::LOW :
    {
      w << "low";
      break;
    }
    case bounding_mode::HIGH :
    {
      w << "high";
      break;
    }
    default:
      break;
  }
  return w;


}
fmt::BasicWriter<char>& operator<<(fmt::BasicWriter<char>& w, const ossia::domain& d)
{
  // TODO
  return w;
}

}
}
