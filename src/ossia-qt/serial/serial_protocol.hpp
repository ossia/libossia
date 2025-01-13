#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/detail/variant.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>
#include <ossia/network/sockets/line_framing.hpp>
#include <ossia/network/sockets/no_framing.hpp>
#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/network/sockets/size_prefix_framing.hpp>
#include <ossia/network/sockets/slip_framing.hpp>

#include <ossia-qt/js_utilities.hpp>

#include <boost/asio/error.hpp>
#include <boost/asio/streambuf.hpp>

#include <QObject>
#include <QQmlComponent>
#include <QSerialPort>
#include <QThread>

#include <span>
#include <verdigris>

class QQmlEngine;
class QQmlComponent;

namespace ossia
{
namespace net
{
struct serial_parameter_data_base
{
  serial_parameter_data_base() = default;
  serial_parameter_data_base(const serial_parameter_data_base&) = default;
  serial_parameter_data_base(serial_parameter_data_base&&) = default;
  serial_parameter_data_base& operator=(const serial_parameter_data_base&) = default;
  serial_parameter_data_base& operator=(serial_parameter_data_base&&) = default;
  serial_parameter_data_base(const QJSValue& val)
  {
    request = val.property("request");
    if(!request.isString() && !request.isCallable())
    {
      request = val.property("name");
    }

    if(val.hasProperty("osc_address"))
      osc_address = val.property("osc_address").toString();
  }

  QJSValue request;
  QString osc_address;
};
struct serial_parameter_data final
    : public parameter_data
    , public serial_parameter_data_base
{
  using base_data_type = serial_parameter_data_base;
  serial_parameter_data() = default;
  serial_parameter_data(const serial_parameter_data&) = default;
  serial_parameter_data(serial_parameter_data&&) = default;
  serial_parameter_data& operator=(const serial_parameter_data&) = default;
  serial_parameter_data& operator=(serial_parameter_data&&) = default;

  serial_parameter_data(const std::string& name)
      : parameter_data{name}
  {
  }

  serial_parameter_data(const QJSValue& val)
      : parameter_data{ossia::qt::make_parameter_data(val)}
      , serial_parameter_data_base{val}
  {
  }

  bool valid() const noexcept
  {
    return request.isString() || request.isCallable() || type;
  }
};

struct no_framing_socket : ossia::net::serial_socket<no_framing>
{
  using serial_socket::serial_socket;
};
struct size_framing_socket : ossia::net::serial_socket<size_prefix_framing>
{
  using serial_socket::serial_socket;
};
struct slip_framing_socket : ossia::net::serial_socket<slip_framing>
{
  using serial_socket::serial_socket;
};
struct line_framing_socket : ossia::net::serial_socket<line_framing>
{
  using serial_socket::serial_socket;
};

using framed_serial_socket = ossia::slow_variant<
    no_framing_socket, size_framing_socket, slip_framing_socket, line_framing_socket>;

struct serial_protocol_configuration
{
  ossia::net::framing framing;
  std::string line_framing_delimiter;

  serial_configuration transport;
};

class OSSIA_EXPORT serial_wrapper final : public QObject
{
  W_OBJECT(serial_wrapper)

public:
  serial_wrapper(
      const network_context_ptr& ctx,
      const ossia::net::serial_protocol_configuration& port);
  ~serial_wrapper() noexcept;

  void write(QByteArray arg_1) E_SIGNAL(OSSIA_EXPORT, write, arg_1);
  void read(QString txt, QByteArray raw) E_SIGNAL(OSSIA_EXPORT, read, txt, raw);

  void on_write(const QByteArray& b) noexcept;
  W_SLOT(on_write, (const QByteArray&))

  void on_write(std::string_view b) noexcept;

  void on_read(const QByteArray& arr);
  W_SLOT(on_read)

  void close();

private:
  framed_serial_socket make_socket(
      const network_context_ptr& ctx,
      const ossia::net::serial_protocol_configuration& port);
  framed_serial_socket m_socket;
  bool m_open{};
};

class serial_protocol;
struct serial_protocol_object;

using serial_parameter = wrapped_parameter<serial_parameter_data>;
using serial_node = ossia::net::wrapped_node<serial_parameter_data, serial_parameter>;
class OSSIA_EXPORT serial_protocol final
    : public QObject
    , public ossia::net::protocol_base
{
public:
  // Param : the name of the serial port
  serial_protocol(
      const ossia::net::network_context_ptr&, const QByteArray& code,
      const ossia::net::serial_configuration& bot);
  ~serial_protocol() override;

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base&, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  void set_device(ossia::net::device_base& dev) override;
  void stop() override;

  static serial_parameter_data read_data(const QJSValue& js) { return js; }

  static serial_protocol_object load_serial_object_from_qml(
      serial_protocol& proto, const ossia::net::network_context_ptr& ctx,
      const ossia::net::serial_configuration& cfg);
  void create(
      const ossia::net::network_context_ptr&,
      const ossia::net::serial_configuration& cfg);
  void on_read(const QString& txt, const QByteArray&);
  void do_write(const ossia::net::parameter_base&, const ossia::value& v);
  void do_write_osc(const serial_parameter&, const ossia::value& v);
  void do_write_osc_bundle(
      std::span<std::pair<const serial_parameter*, const ossia::value*>> vec);
  void do_write_osc_impl(
      const serial_parameter& addr, const ossia::value& v, std::string& str);

private:
  void startup_engine();
  void create_object(QQmlComponent::Status status);

  ossia::net::network_context_ptr m_context;
  ossia::net::serial_configuration m_cfg;

  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  ossia::net::device_base* m_device{};
  QObject* m_object{};
  QJSValue m_jsObj{};
  QJSValue m_onTextMessage{};
  QJSValue m_onBinaryMessage{};
  QJSValue m_onRead{};
  std::shared_ptr<serial_wrapper> m_port;
  QByteArray m_code;

  QObject* m_threadWorker{};
  QThread m_thread{};

  coalescing_queue m_queue;
  std::optional<double> m_coalesce{};
  bool m_osc{};
};
using serial_device = ossia::net::wrapped_device<serial_node, serial_protocol>;

class OSSIA_EXPORT Serial : public QObject
{
};

}
}
