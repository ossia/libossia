#pragma once

#include "ext.h"

#include <ossia/network/base/node.hpp>

static const constexpr int MAX_NUM_ITEMS = 256;

namespace ossia
{
namespace max_binding
{
class matcher;
struct search_filter
{
  t_symbol* m_filter_type[MAX_NUM_ITEMS] = { nullptr };
  long m_filter_type_size{};
  t_symbol* m_filter_tags[MAX_NUM_ITEMS] = { nullptr };
  long m_filter_tags_size{};
  t_symbol* m_filter_modes[3] = {nullptr};
  long m_filter_modes_size{};
  long m_filter_visible{0};
  long m_filter_terminal{0};

  bool filter(const ossia::net::node_base& node);

  template <typename T>
  static void setup_attribute(t_class* c)
  {
    CLASS_ATTR_SYM_VARSIZE(
        c, "type", 0, T, m_filter_type, m_filter_type_size, MAX_NUM_ITEMS);
    CLASS_ATTR_LABEL(c, "type", 0, "Type of object to search");

    CLASS_ATTR_SYM_VARSIZE(
        c, "tags", 0, T, m_filter_tags, m_filter_tags_size, MAX_NUM_ITEMS);
    CLASS_ATTR_LABEL(c, "tags", 0, "Filter by tags");

    CLASS_ATTR_SYM_VARSIZE(c, "mode", 0, T, m_filter_modes, m_filter_modes_size, 3);
    CLASS_ATTR_LABEL(c, "mode", 0, "Filter by access mode");

    CLASS_ATTR_LONG(c, "visible", 0, T, m_filter_visible);
    CLASS_ATTR_LABEL(
        c, "visible", 0,
        "Filter by visibility (0 means no filtering, 1 only visible, 2 only hidden");
    CLASS_ATTR_FILTER_CLIP(c, "visible", 0, 2);

    CLASS_ATTR_LONG(c, "terminal", 0, T, m_filter_terminal);
    CLASS_ATTR_STYLE_LABEL(c, "terminal", 0, "onoff", "Only show terminal nodes");
  }
};

struct search_sort_filter : search_filter
{
  long m_depth{0}; // 0 means no depth filtering

  t_symbol* m_sort{};
  t_symbol* m_format{};

  std::vector<ossia::net::node_base*>
  sort_and_filter(std::vector<std::shared_ptr<matcher>>& matchers);

  template <typename T>
  static void setup_attribute(t_class* c)
  {
    search_filter::setup_attribute<T>(c);

    CLASS_ATTR_LONG(c, "depth", 0, T, m_depth);
    CLASS_ATTR_LABEL(c, "depth", 0, "Limit exploration depth");
    CLASS_ATTR_FILTER_MIN(c, "depth", 0);

    CLASS_ATTR_SYM(c, "format", 0, T, m_format);
    CLASS_ATTR_LABEL(c, "format", 0, "Format to output the namespace in");
    CLASS_ATTR_ENUM(c, "format", 0, "list jit.cellblock");

    CLASS_ATTR_SYM(c, "sort", 0, T, m_sort);
    CLASS_ATTR_LABEL(c, "sort", 0, "Sorting");
    CLASS_ATTR_ENUM(c, "sort", 0, "none alphabetic priority");
  }
};

void dump_node_list(
    void* outlet, const std::vector<ossia::net::node_base*>& nodes, t_symbol* format,
    t_symbol* prefix = nullptr);
} // namespace max
} // namespace ossia
