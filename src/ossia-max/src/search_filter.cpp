#include "search_filter.hpp"
#include "ext_obex_util.h"
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/detail/algorithms.hpp>
#include "utils.hpp"

namespace ossia {
namespace max {

bool search_filter::filter(const ossia::net::node_base& node)
{
  if(m_filter_tags_size > 0)
  {
    bool match = true;
    const auto& tags_opt = ossia::net::get_tags(node.get_extended_attributes());
    if(!tags_opt.has_value())
      return true;

    const std::vector<std::string>& tags = *tags_opt;

    for(int i = 0; i < m_filter_tags_size; i++)
    {
      std::string tag(m_filter_tags[i]->s_name);

      if(ossia::find(tags, tag) == tags.end())
      {
        match = false;
        break;
      }
    }
    if(!match)
      return true;
  }

  if(m_filter_visible > 0)
  {
    auto hidden = ossia::net::get_hidden(node.get_extended_attributes());
    switch(m_filter_visible)
    {
      case 1:
        if(hidden) return true;
        break;
      case 2:
        if(!hidden) return true;
        break;
      default:
          ;
    }
  }

  if(m_filter_modes_size > 0)
  {
    const auto& access_opt = ossia::net::get_access_mode(node);
    if(!access_opt.has_value())
      return true;
    bool match = false;
    for(int i = 0; i<m_filter_modes_size; i++)
    {
      if(symbol2access_mode(m_filter_modes[i]) == *access_opt)
      {
        match = true;
        break;
      }
    }
    if(!match)
      return true;
  }

  return false;
}

} // namespace max
} // namespace ossia
