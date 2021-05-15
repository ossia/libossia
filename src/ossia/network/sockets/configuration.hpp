#pragma once
#include <cstdint>
#include <string>
#include <optional>

namespace ossia::net
{
enum class framing
{
  size_prefix, slip
};

struct fd_configuration
{
  std::string fd;
};

struct send_fd_configuration : fd_configuration
{
};
struct receive_fd_configuration : fd_configuration
{
};

struct socket_configuration
{
  std::string host;
  uint16_t port;
};

struct send_socket_configuration : socket_configuration
{
};
struct receive_socket_configuration : socket_configuration
{
};

struct double_fd_configuration
{
  std::optional<receive_fd_configuration> local;
  std::optional<send_fd_configuration> remote;
};

struct double_socket_configuration
{
  std::optional<receive_socket_configuration> local;
  std::optional<send_socket_configuration> remote;
};

struct serial_configuration
{
  // the serial device name ("COM1", "/dev/ttyUSB1"...)
  std::string port;

  int baud_rate {19200};
  int character_size {8};
  enum
  {
    no_flow_control,
    software,
    hardware
  } flow_control {no_flow_control};
  enum
  {
    no_parity,
    odd,
    even
  } parity {no_parity};
  enum
  {
    one,
    onepointfive,
    two
  } stop_bits {one};
};

struct ws_client_configuration
{
  std::string url;
};

struct ws_server_configuration
{
  int port {};
};

// first / second: the unix sockets name.
// Must be reverted between host and mirror as they are symmetrical.
struct unix_dgram_configuration
    : double_fd_configuration
{
};

struct unix_stream_configuration
    : fd_configuration
{
};

struct udp_configuration
    : double_socket_configuration
{
};

struct tcp_configuration
    : socket_configuration
{
};
}
