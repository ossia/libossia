#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <QtQml>
#include <QNetworkAccessManager>
#include <deque>

namespace ossia
{
namespace net
{
class http_device;
class http_node;
class http_address;

struct http_address_data
{
  QString name;
  QString request;
  QJSValue answerer;
  boost::optional<ossia::val_type> t;
};

class OSSIA_EXPORT http_protocol final :
    public QObject,
    public ossia::net::protocol_base
{
  Q_OBJECT
private:
  QQmlEngine *mEngine{};
  QQmlComponent* mComponent{};

  QNetworkAccessManager* mAccessManager{};

  QByteArray mCode;
  http_device* mDevice{};
  QList<std::pair<QNetworkReply*, const http_address*>> mReplies;
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

signals:
  void sig_push(const http_address*);
private slots:
  void slot_push(const http_address*);
private:
  void create_device(QJSValue);
  void create_node_rec(QJSValue js, http_node& parent);
  void apply_reply(QJSValue);
};

class OSSIA_EXPORT http_node :
    public ossia::net::node_base
{
  QString mName;
  ossia::net::http_device& mDevice;
  ossia::net::http_node* mParent{};
  std::unique_ptr<ossia::net::http_address> mAddress;

  friend class http_protocol;
public:
  http_node(http_address_data dat, ossia::net::http_device& aDevice,
      http_node& aParent);

  http_node(http_address_data dat, ossia::net::http_device& aDevice);

  ~http_node();

  device_base& getDevice() const override;
  node_base* getParent() const override;

  std::string getName() const override;
  node_base& setName(std::string) override;

  address_base* getAddress() const override;
  address_base* createAddress(val_type = val_type::IMPULSE) override;
  bool removeAddress() override;

private:
  std::unique_ptr<node_base> makeChild(const std::string& name) override;

  void removingChild(node_base& node_base) override;
};


class OSSIA_EXPORT http_device final :
    public ossia::net::device_base,
    public http_node
{
public:
  http_device() = delete;
  http_device(const http_device&) = delete;
  http_device(http_device&&) = delete;
  http_device& operator=(const http_device&) = delete;
  http_device& operator=(http_device&&) = delete;

  http_device(
      std::unique_ptr<ossia::net::http_protocol> protocol_base,
      std::string name);

  const ossia::net::node_base& getRootNode() const override
  {
    return *this;
  }
  ossia::net::node_base& getRootNode() override
  {
    return *this;
  }

  using http_node::getName;
  using http_node::setName;

  ~http_device();
};


class OSSIA_EXPORT http_address final : public ossia::net::generic_address
{
private:
  http_address_data mData;

public:
  http_address(http_address_data data, const ossia::net::node_base& node_base);

  http_address() = delete;
  http_address(const http_address& other) = delete;
  http_address(http_address&& other) = delete;
  http_address& operator=(const http_address& other) = delete;
  http_address& operator=(http_address&& other) = delete;
  ~http_address();

  const http_address_data& data() const
  { return mData; }

private:
};
}
}

Q_DECLARE_METATYPE(ossia::net::http_address*)
