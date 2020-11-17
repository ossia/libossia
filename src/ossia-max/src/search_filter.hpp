#pragma once

#include "ext.h"
#include <ossia/network/base/node.hpp>

static const constexpr int MAX_NUM_ITEMS = 256;

namespace ossia {
namespace max {
struct search_filter
{
  t_symbol* m_filter_types[MAX_NUM_ITEMS];
  long m_filter_types_size{};
  t_symbol* m_filter_tags[MAX_NUM_ITEMS];
  long m_filter_tags_size{};
  t_symbol* m_filter_modes[3];
  long m_filter_modes_size{};
  long m_filter_visible{0};

  bool filter(const ossia::net::node_base& node);

  template<typename T>
  static void setup_attribute(t_class* c)
  {
    CLASS_ATTR_SYM_VARSIZE(c, "types", 0, T, m_filter_types, m_filter_types_size, MAX_NUM_ITEMS);
    CLASS_ATTR_LABEL(c, "types", 0, "Types of object to search");

    CLASS_ATTR_SYM_VARSIZE(c, "tags", 0, T, m_filter_tags, m_filter_tags_size, MAX_NUM_ITEMS);
    CLASS_ATTR_LABEL(c, "tags", 0, "Filter by tags");

    CLASS_ATTR_SYM_VARSIZE(c, "mode", 0, T, m_filter_modes, m_filter_modes_size, 3);
    CLASS_ATTR_LABEL(c, "mode", 0, "Filter by access mode");

    CLASS_ATTR_LONG(c, "visible", 0, T, m_filter_visible);
    CLASS_ATTR_LABEL(c, "visible", 0, "Filter by visibility (0 means no filtering, 1 only visible, 2 only hidden");
    CLASS_ATTR_FILTER_CLIP(c, "visible", 0, 2);
  }
};
} // namespace max
} // namespace ossia
