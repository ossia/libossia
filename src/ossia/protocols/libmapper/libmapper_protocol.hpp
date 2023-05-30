#pragma once

#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/variant.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/device_parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <concurrentqueue.h>

#include <vector>

typedef void* mpr_graph;
typedef void* mpr_device;
typedef void* mpr_signal;
typedef void* mpr_link;
typedef void* mpr_map;
typedef void* mpr_slot;
typedef void* mpr_network;
typedef void* mpr_database;
typedef void* mpr_queue;
typedef int mpr_signal_group;

namespace ossia::net
{

class OSSIA_EXPORT libmapper_server_protocol final : public ossia::net::protocol_base
{
public:
  libmapper_server_protocol();
  libmapper_server_protocol(const libmapper_server_protocol&) = delete;
  libmapper_server_protocol(libmapper_server_protocol&&) = delete;
  libmapper_server_protocol& operator=(const libmapper_server_protocol&) = delete;
  libmapper_server_protocol& operator=(libmapper_server_protocol&&) = delete;
  ~libmapper_server_protocol() override;

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;
  void stop() override;

private:
  template <typename OssiaType, typename LibmapperType>
  void create_1d_parameter(const parameter_base& p);
  template <std::size_t N>
  void create_array_parameter(const parameter_base& p);
  void create_vector_parameter(const parameter_base& p);
  friend struct libmapper_create_param;
  friend struct libmapper_apply_control;

  void on_nodeCreated(const ossia::net::node_base&);
  void on_nodeRemoved(const ossia::net::node_base&);
  void on_parameterCreated(const ossia::net::parameter_base&);
  void on_parameterRemoved(const ossia::net::parameter_base&);
  void on_parameterChanged(const ossia::net::parameter_base&);
  void on_attributeChanged(const ossia::net::node_base&, std::string_view attr);
  void on_nodeRenamed(const ossia::net::node_base& n, std::string oldname);

  void execThread();

  ossia::net::device_base* m_device{};

  mpr_device m_mapper_dev{};

  ossia::hash_map<const ossia::net::parameter_base*, mpr_signal> m_inputMap;
  ossia::hash_map<const ossia::net::parameter_base*, mpr_signal> m_outputMap;

  std::thread m_thread;
  std::atomic_bool m_running{};

  struct Message
  {
    mpr_signal sig;
    ossia::value value;
  };

  moodycamel::ConcurrentQueue<Message> m_sendQueue;

  struct CreateSignal
  {
    const ossia::net::parameter_base* param;
  };
  struct RemoveSignal
  {
    mpr_signal sig;
  };
  using ControlMessage = ossia::variant<CreateSignal, RemoveSignal>;
  moodycamel::ConcurrentQueue<ControlMessage> m_ctlQueue;

  std::atomic_int m_editSemaphore{};
};

class OSSIA_EXPORT libmapper_client_protocol final : public ossia::net::protocol_base
{
public:
  explicit libmapper_client_protocol(std::string device);
  libmapper_client_protocol(const libmapper_client_protocol&) = delete;
  libmapper_client_protocol(libmapper_client_protocol&&) = delete;
  libmapper_client_protocol& operator=(const libmapper_client_protocol&) = delete;
  libmapper_client_protocol& operator=(libmapper_client_protocol&&) = delete;
  ~libmapper_client_protocol() override;

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update();
  bool update(ossia::net::node_base&) override;
  void stop() override;

  void poll(int);

private:
  ossia::net::device_base* m_device{};

  std::string m_mapper_dev_name;

  mpr_graph m_db{};
  mpr_device m_mapper_dev{};

  ossia::hash_map<const ossia::net::parameter_base*, mpr_signal> m_inputMap;
  ossia::hash_map<const ossia::net::parameter_base*, mpr_signal> m_outputMap;
};
}
