#include <ossia/network/base/node.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/detail/html_writer.hpp>

#include <fmt/format.h>
#include <ossia/network/value/format_value.hpp>

namespace ossia::oscquery
{

html_builder::~html_builder()
{
}

static_html_builder::~static_html_builder()
{
}

struct static_html_builder_impl
{
  fmt::memory_buffer& w;
  void build_header(ossia::net::node_base& node)
  {
    fmt::format_to(w, R"_(<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>OSCQuery server</title>
</head>
<body>)_");
  }

  void build_footer()
  {
    fmt::format_to(w, R"_(</body></html>)_");
  }

  void build_node(ossia::net::node_base& node)
  {
    if (auto p = node.get_parameter())
    {
      fmt::format_to(w, "\n<p>");
      fmt::format_to(w, "<b>{}</b> : {}", node.osc_address(), p->value());

      auto& dom = p->get_domain();
      auto min = dom.get_min();
      if (min.valid())
        fmt::format_to(w, " ; <b>Min</b>: {}", min);

      auto max = dom.get_max();
      if (max.valid())
        fmt::format_to(w, " ; <b>Max</b>: {}", max);

      if (auto u = p->get_unit())
        fmt::format_to(w, " ; <b>Unit</b>: {}",  ossia::get_pretty_unit_text(u));
      fmt::format_to(w, "</p>");
    }

    for (auto cld : node.children_copy())
    {
      build_node(*cld);
    }
  }
};

std::string static_html_builder::build_tree(ossia::net::node_base& node)
{
  fmt::memory_buffer w;
  w.reserve(150000);

  static_html_builder_impl bld{w};
  bld.build_header(node);
  bld.build_node(node);
  bld.build_footer();

  return std::string(w.data(), w.size());
}
}
