#pragma once

#include <ossia/detail/logger.hpp>

#include <optional>
#include <string>

namespace ossia
{
struct resolved_url
{
  std::string host;
  std::string port;
  int protocol{}; // tcp, udp
  int family{};   // v4, v6
};

template <typename Proto>
OSSIA_EXPORT std::optional<resolved_url>
resolve_sync_v4(const std::string_view host, const std::string_view port);

struct splitted_host_and_port
{
  std::string_view host;
  std::string_view port;
};

OSSIA_EXPORT
splitted_host_and_port url_to_host_and_port(std::string_view url);
}
