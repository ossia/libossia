#include "serial_protocol.hpp"
#include <ossia/network/serial/serial_address.hpp>
#include <ossia/network/serial/serial_device.hpp>
#include <ossia/network/serial/serial_node.hpp>
#include <sstream>
#include <iomanip>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>

namespace ossia
{
namespace net
{
serial_protocol::serial_protocol(
    const QByteArray& code,
    const QSerialPortInfo& bot):
  mEngine{new QQmlEngine},
  mComponent{new QQmlComponent{mEngine}},
  mSerialPort{bot}
{
  connect(mComponent, &QQmlComponent::statusChanged,
          this, [=] (QQmlComponent::Status status)
  {
    if(!mDevice)
      return;

    switch(status)
    {
      case QQmlComponent::Status::Ready:
      {
        auto item = mComponent->create();
        item->setParent(mEngine->rootContext());

        QVariant ret;
        QMetaObject::invokeMethod(item, "createTree", Q_RETURN_ARG(QVariant, ret));
        create_device<serial_device, serial_node, serial_protocol>(*mDevice, ret.value<QJSValue>());

        return;
      }
      case QQmlComponent::Status::Loading:
        return;
      case QQmlComponent::Status::Null:
      case QQmlComponent::Status::Error:
        qDebug() << mComponent->errorString();
        return;
    }
  }
  );
}

bool serial_protocol::pull(address_base&)
{
  return false;
}

bool serial_protocol::push(const ossia::net::address_base& addr)
{
  auto& ad = dynamic_cast<const serial_address&>(addr);
    std::stringstream s;
    s << addr.getNode().getName();

    switch(addr.getValueType())
    {
        case ossia::val_type::FLOAT:
            s << " " << std::setprecision(4) << std::to_string(ad.getValue().get<float>());
            break;
        case ossia::val_type::INT:
            s << " " << std::to_string(ad.getValue().get<int32_t>());
            break;
        case ossia::val_type::IMPULSE:
            break;
        default:
            throw;
    }

    s << '\n';

    auto str = s.str();
    qDebug() << QString::fromStdString(s.str());
    mSerialPort.write(QByteArray(str.c_str()));
    return false;
}

bool serial_protocol::observe(address_base&, bool)
{
  return false;
}

bool serial_protocol::update(node_base& node_base)
{
  return true;
}

serial_protocol::~serial_protocol()
{

}


}
}
