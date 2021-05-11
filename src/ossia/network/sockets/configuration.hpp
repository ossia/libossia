#pragma once
#include <string>
#include <cstdint>

namespace ossia::net
{

struct fd_configuration {
  std::string fd;
};

struct socket_configuration {
  std::string host;
  uint16_t port;
};

struct double_fd_configuration {
  fd_configuration first;
  fd_configuration second;
};

struct double_socket_configuration {
  socket_configuration local;
  socket_configuration remote;
};

struct serial_configuration {
  // the serial device name ("COM1", "/dev/ttyUSB1"...)
  std::string port;
};

struct ws_client_configuration {
  std::string url;
};

struct ws_server_configuration {
  int port{};
};

// first / second: the unix sockets name.
// Must be reverted between host and mirror as they are symmetrical.
using unix_dgram_configuration = double_fd_configuration;

// read_fd: the unix socket name. Full-duplex.
using unix_stream_configuration = fd_configuration;

using udp_configuration = double_socket_configuration;
using tcp_configuration = socket_configuration;
}
