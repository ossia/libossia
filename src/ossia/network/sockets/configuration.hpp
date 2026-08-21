#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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

  std::optional<int> multicast_ttl;
  std::string multicast_interface{};
  std::optional<bool> multicast_loopback;
};
struct inbound_socket_configuration
{
  std::string bind{"0.0.0.0"};
  uint16_t port{};
  std::string multicast_group{};
  std::string multicast_interface{"0.0.0.0"};
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

// CAN is only implemented on top of SocketCAN for now, so ossia::net::can_socket
// itself is Linux-only -- but the configuration is plain data and exists
// everywhere, exactly like the POSIX-only unix socket configurations above. That
// way the protocol layers that carry a configuration around do not have to be
// guarded themselves, only the code that actually opens a socket.
struct can_filter_configuration
{
  // Both fields are raw SocketCAN values: `id` may carry CAN_EFF_FLAG,
  // CAN_RTR_FLAG and CAN_INV_FILTER, and the kernel keeps a frame when
  // `received_id & mask == id & mask`.
  uint32_t id{};
  uint32_t mask{};
};

struct can_configuration
{
  // The netdev name of the CAN interface: "can0", "vcan0", "slcan0"...
  // Not spelled `interface`: that is a macro (`struct`) on Windows, pulled in by
  // <objbase.h> through most of the Win32 headers.
  std::string interface_name;

  // Enable CAN FD (CAN_RAW_FD_FRAMES): allows payloads of up to 64 bytes.
  // Note that this is not a mode switch -- once enabled, classic frames and FD
  // frames both arrive on the same socket, and the socket keeps being able to
  // send classic frames.
  bool fd{false};

  // Kernel defaults, spelled out. Together they mean: frames written by this
  // socket are looped back to the *other* sockets bound to the interface, but
  // not to the socket that sent them. Disabling loopback is a per-socket switch
  // that turns off the loopback of that socket's own frames to everyone.
  bool loopback{true};
  bool receive_own_messages{false};

  // Receive error frames (CAN_RAW_ERR_FILTER). Those are not bus traffic: the
  // kernel synthesizes them to report controller/bus conditions, and they are
  // delivered with CAN_ERR_FLAG set. See can_message::error.
  bool error_frames{false};

  // Empty means "receive every frame". Note that a non-empty list is applied by
  // the kernel per socket, so several sockets on the same interface each get
  // their own view of the bus.
  std::vector<can_filter_configuration> filters;
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
