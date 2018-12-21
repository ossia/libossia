#pragma once
#include <ossia/network/base/protocol.hpp>
#include <wobjectdefs.h>
#include <ossia-qt/js_utilities.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>

#include <QByteArray>
#include <QJSValue>
#include <QList>
#include <QObject>

class QQmlEngine;
class QQmlComponent;
class QWebSocket;
class QNetworkReply;

namespace ossia
{
namespace net
{

struct ws_generic_client_parameter_data_base
{
  ws_generic_client_parameter_data_base() = default;
  ws_generic_client_parameter_data_base(
      const ws_generic_client_parameter_data_base&)
      = default;
  ws_generic_client_parameter_data_base(ws_generic_client_parameter_data_base&&)
      = default;
  ws_generic_client_parameter_data_base&
  operator=(const ws_generic_client_parameter_data_base&)
      = default;
  ws_generic_client_parameter_data_base&
  operator=(ws_generic_client_parameter_data_base&&)
      = default;
  ws_generic_client_parameter_data_base(const QJSValue& val)
      : request{val.property("request")}
      , openListening{val.property("openListening")}
      , closeListening{val.property("closeListening")}
  {
  }

  QJSValue request;
  QJSValue openListening;
  QJSValue closeListening;
};

struct ws_generic_client_parameter_data
    : public parameter_data,
      public ws_generic_client_parameter_data_base
{
  using base_data_type = ws_generic_client_parameter_data_base;
  ws_generic_client_parameter_data() = default;
  ws_generic_client_parameter_data(const ws_generic_client_parameter_data&)
      = default;
  ws_generic_client_parameter_data(ws_generic_client_parameter_data&&) = default;
  ws_generic_client_parameter_data&
  operator=(const ws_generic_client_parameter_data&)
      = default;
  ws_generic_client_parameter_data& operator=(ws_generic_client_parameter_data&&)
      = default;

  ws_generic_client_parameter_data(const std::string& name) : parameter_data{name}
  {
  }

  ws_generic_client_parameter_data(const QJSValue& val)
      : parameter_data{ossia::qt::make_parameter_data(val)}
      , ws_generic_client_parameter_data_base{val}
  {
  }

  bool valid() const noexcept { return !request.isNull() || type; }
};

using ws_generic_client_parameter = wrapped_parameter<ws_generic_client_parameter_data>;
using ws_generic_client_node = ossia::net::wrapped_node<ws_generic_client_parameter_data, ws_generic_client_parameter>;

class OSSIA_EXPORT ws_generic_client_protocol final
    : public QObject,
      public ossia::net::protocol_base
{
  W_OBJECT(ws_generic_client_protocol)

public:
  ws_generic_client_protocol(const QString& addr, QByteArray code);

  ws_generic_client_protocol(const ws_generic_client_protocol&) = delete;
  ws_generic_client_protocol(ws_generic_client_protocol&&) = delete;
  ws_generic_client_protocol& operator=(const ws_generic_client_protocol&)
      = delete;
  ws_generic_client_protocol& operator=(ws_generic_client_protocol&&) = delete;

  ~ws_generic_client_protocol() override;

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::parameter_base& parameter_base) override;

  bool push(const ossia::net::parameter_base& parameter_base, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override;

  void set_device(ossia::net::device_base& dev) override;

  static ws_generic_client_parameter_data read_data(const QJSValue& js)
  {
    return js;
  }
public:
  void sig_push(const ws_generic_client_parameter* arg_1, const ossia::value& v) E_SIGNAL(OSSIA_EXPORT, sig_push, arg_1, v);

private:
  void slot_push(const ws_generic_client_parameter*, const ossia::value& v); W_SLOT(slot_push);

private:
  void apply_reply(QJSValue);

  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};
  QObject* m_object{};

  QWebSocket* m_websocket{};

  QByteArray m_code;
  ossia::net::device_base* m_device{};
  QList<std::pair<QNetworkReply*, const ws_generic_client_parameter*>> m_replies;
};

using ws_generic_client_device = ossia::net::wrapped_device<ws_generic_client_node, ws_generic_client_protocol>;
class OSSIA_EXPORT WS : public QObject
{
};
}
}

Q_DECLARE_METATYPE(ossia::net::ws_generic_client_parameter*)
W_REGISTER_ARGTYPE(ossia::net::ws_generic_client_parameter*)
W_REGISTER_ARGTYPE(const ossia::net::ws_generic_client_parameter*)
