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
public:
  http_protocol(std::string code);

  http_protocol(const http_protocol&) = delete;
  http_protocol(http_protocol&&) = delete;
  http_protocol& operator=(const http_protocol&) = delete;
  http_protocol& operator=(http_protocol&&) = delete;

  ~http_protocol();

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::address_base& address_base) override;

  bool push(const ossia::net::address_base& address_base) override;

  bool observe(ossia::net::address_base& address_base, bool enable) override;
};

class OSSIA_EXPORT http_node :
    public ossia::net::node_base
{
  QString mName;
  ossia::net::http_device& mDevice;
  ossia::net::http_node* mParent{};
  std::unique_ptr<ossia::net::http_address> mAddress;

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
private:
  std::string mDeviceName;

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

  std::string getName() const override { return mDeviceName; }
  using http_node::setName;

  ~http_device();
};


class OSSIA_EXPORT http_address final : public ossia::net::generic_address
{
private:
  http_address_data mData;

public:
  http_address(http_address_data data, const ossia::net::node_base& node_base);

  ~http_address();

  const http_address_data& data() const
  { return mData; }

private:
};
}
}
