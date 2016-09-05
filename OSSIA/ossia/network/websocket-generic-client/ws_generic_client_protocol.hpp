#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_address.hpp>

#include <QList>
#include <QByteArray>
#include <QJSValue>
#include <QObject>

class QQmlEngine;
class QQmlComponent;
class QWebSocket;
class QNetworkReply;

namespace ossia
{
namespace net
{
class ws_generic_client_device;
class ws_generic_client_node;
class ws_generic_client_address;

class OSSIA_EXPORT ws_generic_client_protocol final :
    public QObject,
    public ossia::net::protocol_base
{
  Q_OBJECT

public:
  ws_generic_client_protocol(QByteArray code);

  ws_generic_client_protocol(const ws_generic_client_protocol&) = delete;
  ws_generic_client_protocol(ws_generic_client_protocol&&) = delete;
  ws_generic_client_protocol& operator=(const ws_generic_client_protocol&) = delete;
  ws_generic_client_protocol& operator=(ws_generic_client_protocol&&) = delete;

  ~ws_generic_client_protocol();

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::address_base& address_base) override;

  bool push(const ossia::net::address_base& address_base) override;

  bool observe(ossia::net::address_base& address_base, bool enable) override;

  void setDevice(ossia::net::device_base& dev) override;

  static ws_generic_client_address_data read_data(const QJSValue& js) { return js; }
signals:
  void sig_push(const ws_generic_client_address*);

private slots:
  void slot_push(const ws_generic_client_address*);

private:
  void apply_reply(QJSValue);

  QQmlEngine *mEngine{};
  QQmlComponent* mComponent{};

  QWebSocket* mWebsocket{};

  QByteArray mCode;
  ws_generic_client_device* mDevice{};
  QList<std::pair<QNetworkReply*, const ws_generic_client_address*>> mReplies;
};

}
}

Q_DECLARE_METATYPE(ossia::net::ws_generic_client_address*)
