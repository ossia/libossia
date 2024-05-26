#pragma once
#include <ossia/detail/variant.hpp>

#include <boost/container/flat_map.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ossia::coap
{

struct link_format
{
  struct resource
  {
    std::string path;
    boost::container::flat_map<
        std::string, ossia::variant<ossia::monostate, int64_t, std::string>>
        options;
  };

  std::vector<resource> resources;
};

}

namespace ossia::coap
{
//! Parser for RFC 6690 CoRE Link Format
std::optional<ossia::coap::link_format> parse_link_format(std::string_view str);
}
