#pragma once
#include <cstdint>
#include <optional>
#include <string>

namespace ossia::net
{
enum class framing
{
  none,
  size_prefix, // 4-byte big-endian (backward compatible)
  slip,
  line_delimiter,
  cobs,
  stx_etx,
  size_prefix_1byte,
  size_prefix_2byte_be,
  size_prefix_2byte_le,
  size_prefix_4byte_le,
  fixed_length
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

struct outbound_socket_configuration
{
  std::string host;
  uint16_t port{};
  bool broadcast{};
};
struct inbound_socket_configuration
{
  std::string bind{"0.0.0.0"};
  uint16_t port{};
};

struct double_fd_configuration
{
  std::optional<receive_fd_configuration> local;
  std::optional<send_fd_configuration> remote;
};

struct double_socket_configuration
{
  std::optional<inbound_socket_configuration> local;
  std::optional<outbound_socket_configuration> remote;
};

struct serial_configuration
{
  // the serial device name ("COM1", "/dev/ttyUSB1"...)
  std::string port;

  int baud_rate{19200};
  int character_size{8};
  enum
  {
    no_flow_control,
    software,
    hardware
  } flow_control{no_flow_control};
  enum
  {
    no_parity,
    odd,
    even
  } parity{no_parity};
  enum
  {
    one,
    onepointfive,
    two
  } stop_bits{one};
};

struct ws_client_configuration
{
  std::string url;
};

struct ws_server_configuration
{
  int port{};
};

// first / second: the unix sockets name.
// Must be reverted between host and mirror as they are symmetrical.
struct unix_dgram_configuration : double_fd_configuration
{
};

struct unix_stream_configuration : fd_configuration
{
};

struct udp_configuration : double_socket_configuration
{
};

struct tcp_client_configuration : outbound_socket_configuration
{
};

struct udp_server_configuration : inbound_socket_configuration
{
};
struct tcp_server_configuration : inbound_socket_configuration
{
};
struct unix_dgram_server_configuration : receive_fd_configuration
{
};
struct unix_stream_server_configuration : receive_fd_configuration
{
};
}
