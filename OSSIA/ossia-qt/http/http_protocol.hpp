#pragma once
#include <ossia/network/base/protocol.hpp>
#include <wobjectdefs.h>
#include <ossia-qt/http/http_parameter.hpp>

#include <QByteArray>
#include <QJSValue>
#include <QList>
#include <QObject>

class QQmlEngine;
class QQmlComponent;
class QNetworkAccessManager;
class QNetworkReply;

namespace ossia
{
namespace net
{
class http_device;
class http_node;
class http_parameter;

class OSSIA_EXPORT http_protocol final
    : public QObject
    , public ossia::net::protocol_base
{
  W_OBJECT(http_protocol)

public:
  http_protocol(QByteArray code);

  http_protocol(const http_protocol&) = delete;
  http_protocol(http_protocol&&) = delete;
  http_protocol& operator=(const http_protocol&) = delete;
  http_protocol& operator=(http_protocol&&) = delete;

  ~http_protocol() override;

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::parameter_base& parameter_base) override;

  bool push(const ossia::net::parameter_base& parameter_base) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override;

  void set_device(ossia::net::device_base& dev) override;

  static http_parameter_data read_data(const QJSValue& js)
  {
    return js;
  }
public:
  void sig_push(const http_parameter* arg_1) E_SIGNAL(OSSIA_EXPORT, sig_push, arg_1);

private:
  void slot_push(const http_parameter*); W_SLOT(slot_push);

private:
  void apply_reply(QJSValue);

  QQmlEngine* m_engine{};
  QQmlComponent* m_component{};

  QNetworkAccessManager* m_access{};

  QByteArray m_code;
  http_device* m_device{};
  QList<std::pair<QNetworkReply*, const http_parameter*>> m_replies;
};
}
}

Q_DECLARE_METATYPE(ossia::net::http_parameter*)
W_REGISTER_ARGTYPE(ossia::net::http_parameter*)
W_REGISTER_ARGTYPE(const ossia::net::http_parameter*)
