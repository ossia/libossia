#pragma once

#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/json_fwd.hpp>
#include <ossia/detail/thread.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/host_info.hpp>

#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include <string>

namespace osc
{
template <typename T>
class sender;
class receiver;
}

namespace oscpack
{
class ReceivedMessage;
class IpEndpointName;
}

namespace ossia
{
namespace net
{
struct parameter_data;
class websocket_client;
}
namespace oscquery
{
struct osc_outbound_visitor;
struct http_client_context;
struct value_read;
struct http_responder;

class OSSIA_EXPORT oscquery_mirror_protocol final : public ossia::net::protocol_base
{
public:
  oscquery_mirror_protocol(std::string host, uint16_t local_osc_port = 10203);
  ~oscquery_mirror_protocol() override;

  bool pull(net::parameter_base&) override;
  std::future<void> pull_async(net::parameter_base&) override;
  void request(net::parameter_base&) override;
  bool push(const net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(const std::vector<ossia::net::full_parameter_data>&) override;
  bool observe(net::parameter_base&, bool) override;
  bool observe_quietly(net::parameter_base&, bool) override;
  bool update(net::node_base& b) override;
  bool echo_incoming_message(
      const ossia::net::message_origin_identifier& id,
      const ossia::net::parameter_base& addr, const value& val) override;

  std::future<void> update_async(net::node_base& b) override;

  void stop() override;
  void set_device(net::device_base& dev) override;
  ossia::net::device_base& get_device() const { return *m_device; }

  /**
   * @brief Run the commands registered in th event queue
   *
   * The data structures are not protected by locks since it may hurt
   * the performance on a tree, and hard to get right.
   * Instead, all the edition operations on a device should happen on a single
   * thread.
   * When edition operations are received from the network, they are put in a
   * queue.
   *
   * Run this function regularly in order to get the update of the device, for
   * instance in the event loop of your application.
   */
  void run_commands();

  /**
   * @brief Provide a callback for whenever an edition command is received
   *
   * This is an alternative way to calling runCommands() : a function
   * can be provided; it will be called every time a new command is received,
   * after it has been pushed.
   * This can be used to put the command in the application's main loop.
   */
  void set_command_callback(std::function<void()>);

  /**
   * @brief Request a new node from the server
   * @param Parent of the new node
   * @param Data of the new node
   */
  void request_add_node(net::node_base&, const ossia::net::parameter_data&);

  /**
   * @brief Request a node removal from the server
   * @param Node to be removed
   */
  void request_remove_node(net::node_base&);

  /**
   * @brief Request a node renaming from the server
   * @param Node to be removed and new name
   */
  void request_rename_node(net::node_base& node, const std::string& new_name);

  /**
   * @brief Define behavior when a node is removed : mark it as zombie if true
   * (default), remove it otherwise.
   * @param zombie_on_removed
   */
  void set_zombie_on_remove(bool zombie_on_remove)
  {
    m_zombie_on_remove = zombie_on_remove;
  }

  /**
   * @brief Get zombie on removed move
   * @return
   */
  bool get_zombie_on_remove() const noexcept { return m_zombie_on_remove; }

  /**
   * @brief Whether a read of a parameter nothing follows asks the server.
   *
   * The server sends the values of what a client listens to, and a parameter
   * with no callback is not listened to: its local value is the last one
   * that came, which may be out of date. With this on, read_followed_value()
   * asks the server for such a value, without waiting for the answer, at
   * most every refresh_interval for an address, and gives the last value
   * known meanwhile. For a host that polls values (a game engine reading
   * them every frame) rather than following them. Off by default: reads
   * are local, as before.
   */
  void set_read_unfollowed(bool b) noexcept { m_readUnfollowed = b; }
  bool get_read_unfollowed() const noexcept { return m_readUnfollowed; }
  static constexpr std::chrono::milliseconds refresh_interval{100};

  //! Asks the server for the value of a parameter nothing follows, without
  //! waiting for it: at most one request in flight for an address, and one
  //! per refresh_interval
  void refresh_unfollowed(const std::string& address);

  //! Whether this client wrote that address within the last 500 ms: the
  //! server may not have it yet
  bool written_recently(const std::string& address);

  host_info get_host_info() const noexcept;

  bool connected() const noexcept override { return m_hasWS; }
  void connect() override;

private:
  friend struct http_answer;
  friend struct http_value_answer;

  void init();
  using connection_handler = std::weak_ptr<void>;
  bool on_WSMessage(connection_handler hdl, const std::string& message);
  bool on_BinaryWSMessage(connection_handler hdl, const std::string& message);
  // A value read, and its answer for the address it was asked for
  void read_value(std::shared_ptr<value_read> read);
  void on_value_read(
      const std::string& address, const std::string& message, bool only_if_differs);
  void
  on_OSCMessage(const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip);

  void cleanup_connections();

  void http_send_message(const std::string& str);
  void http_send_message(const rapidjson::StringBuffer& str);

  void ws_send_message(const std::string& str);
  void ws_send_binary_message(const std::string& str);
  void ws_send_message(const rapidjson::StringBuffer& str);
  bool query_connected();
  void query_stop();

  void on_nodeRenamed(const ossia::net::node_base& n, std::string oldname);

  void start_http();

  void on_ws_disconnected() { m_hasWS = false; }

  std::unique_ptr<osc::sender<oscquery::osc_outbound_visitor>> m_oscSender;
  std::unique_ptr<osc::receiver> m_oscServer;
  std::unique_ptr<ossia::net::websocket_client> m_websocketClient;
  std::atomic_bool m_hasWS{};

  // Listening status of the local software
  net::listened_parameters m_listening;

  ossia::net::device_base* m_device{};

  std::promise<void> m_namespacePromise;

  struct get_osc_promise
  {
    std::promise<void> promise;
    ossia::net::parameter_base* address{};
  };
  // set_read_unfollowed: the reads of a parameter nothing follows, by
  // address (the last request and its answer to come), and the last writes
  // of this client
  std::atomic_bool m_readUnfollowed{false};
  struct unfollowed_read
  {
    std::chrono::steady_clock::time_point asked{};
    std::future<void> answer;
  };
  std::mutex m_unfollowedMutex;
  ossia::hash_map<std::string, unfollowed_read> m_unfollowed;
  ossia::hash_map<std::string, std::chrono::steady_clock::time_point> m_written;
  ossia::spsc_queue<std::function<void()>> m_functionQueue;
  std::function<void()> m_commandCallback;

  ossia::thread m_wsThread;
  std::string m_queryHost;
  std::string m_queryPort;
  std::string m_httpHost;
  int m_osc_port{};

  std::unique_ptr<http_client_context> m_http;
  host_info m_host_info;
  mutable std::mutex m_host_info_mutex;

  ossia::net::message_origin_identifier m_id;

  void set_feedback(bool fb) override;
  std::atomic<bool> m_feedback{true};
  bool m_zombie_on_remove{true};
};

//! The value to give someone who reads a parameter. On a mirror with
//! set_read_unfollowed(true), a parameter nothing follows is asked for (see
//! there); otherwise, and on any other device, its value as it is.
OSSIA_EXPORT ossia::value read_followed_value(ossia::net::parameter_base& param);

//! Use this function to load a device preset in the OSCQuery format.
OSSIA_EXPORT void load_oscquery_device(ossia::net::device_base& dev, std::string json);
}
}
