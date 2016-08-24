#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/http/http_address.hpp>

#include <QList>
#include <QByteArray>
#include <QJSValue>
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
class http_address;

class OSSIA_EXPORT http_protocol final :
    public QObject,
    public ossia::net::protocol_base
{
  Q_OBJECT

public:
  http_protocol(QByteArray code);

  http_protocol(const http_protocol&) = delete;
  http_protocol(http_protocol&&) = delete;
  http_protocol& operator=(const http_protocol&) = delete;
  http_protocol& operator=(http_protocol&&) = delete;

  ~http_protocol();

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::address_base& address_base) override;

  bool push(const ossia::net::address_base& address_base) override;

  bool observe(ossia::net::address_base& address_base, bool enable) override;

  void setDevice(ossia::net::device_base& dev) override;

  static http_address_data read_data(const QJSValue& js) { return js; }
signals:
  void sig_push(const http_address*);

private slots:
  void slot_push(const http_address*);

private:
  void apply_reply(QJSValue);

  QQmlEngine *mEngine{};
  QQmlComponent* mComponent{};

  QNetworkAccessManager* mAccessManager{};

  QByteArray mCode;
  http_device* mDevice{};
  QList<std::pair<QNetworkReply*, const http_address*>> mReplies;
};

}
}

Q_DECLARE_METATYPE(ossia::net::http_address*)
