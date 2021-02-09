#pragma once
#include <string>
#include <cstdint>

namespace ossia::net
{

struct fd_configuration {
  std::string read_fd;
  std::string write_fd;
};

struct socket_configuration {
  std::string local_host;
  std::string remote_host;
  uint16_t local_port;
  uint16_t remote_port;
};

}
