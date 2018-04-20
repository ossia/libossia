#pragma once
#include <ossia-config.hpp>
#include <ossia_export.h>
#include <string>

namespace ossia::net { class node_base; }
namespace ossia::oscquery
{
class OSSIA_EXPORT html_builder
{
public:
  virtual ~html_builder();
  virtual std::string build_tree(ossia::net::node_base& node) = 0;
};

class OSSIA_EXPORT static_html_builder final: public html_builder
{
public:
  ~static_html_builder() override;
  std::string build_tree(ossia::net::node_base& node) override;
};

}
