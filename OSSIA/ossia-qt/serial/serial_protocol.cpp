// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "serial_protocol.hpp"
#include <QDebug>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <iomanip>
#include <ossia-qt/serial/serial_parameter.hpp>
#include <ossia-qt/serial/serial_device.hpp>
#include <ossia-qt/serial/serial_node.hpp>
#include <sstream>

namespace ossia
{
namespace net
{

serial_wrapper::~serial_wrapper()
{
    
}

serial_protocol::serial_protocol(
    const QByteArray& code, const QSerialPortInfo& bot)
    : mEngine{new QQmlEngine}
    , mComponent{new QQmlComponent{mEngine}}
    , mSerialPort{bot}
    , mCode{code}
{
  connect(
      mComponent, &QQmlComponent::statusChanged, this,
      [=](QQmlComponent::Status status) {
        qDebug() << status;
        qDebug() << mComponent->errorString();
        if (!mDevice)
          return;

        switch (status)
        {
          case QQmlComponent::Status::Ready:
          {
            auto item = mComponent->create();
            item->setParent(mEngine->rootContext());

            QVariant ret;
            QMetaObject::invokeMethod(
                item, "createTree", Q_RETURN_ARG(QVariant, ret));
            qt::create_device<serial_device, serial_node, serial_protocol>(
                *mDevice, ret.value<QJSValue>());

            return;
          }
          case QQmlComponent::Status::Loading:
            return;
          case QQmlComponent::Status::Null:
          case QQmlComponent::Status::Error:
            qDebug() << mComponent->errorString();
            return;
        }
      });
}

bool serial_protocol::pull(parameter_base&)
{
  return false;
}

bool serial_protocol::push(const ossia::net::parameter_base& addr)
{
  auto& ad = dynamic_cast<const serial_parameter&>(addr);
  auto str = ad.data().request;
  switch (addr.get_value_type())
  {
    case ossia::val_type::FLOAT:
      str.replace("$val", QString::number(ad.getValue().get<float>(), 'g', 4));
      break;
    case ossia::val_type::INT:
      str.replace("$val", QString::number(ad.getValue().get<int32_t>()));
      break;
    case ossia::val_type::IMPULSE:
      break;
    default:
      throw std::runtime_error("serial_protocol::push: bad type");
  }

  str += '\n';
  qDebug() << str;
  mSerialPort.write(str.toUtf8());
  return false;
}

bool serial_protocol::push_raw(const full_parameter_data& parameter_base)
{ return false; }

bool serial_protocol::observe(ossia::net::parameter_base&, bool)
{
  return false;
}

bool serial_protocol::update(ossia::net::node_base& node_base)
{
  return true;
}

void serial_protocol::set_device(device_base& dev)
{
  if (auto htdev = dynamic_cast<serial_device*>(&dev))
  {
    mDevice = htdev;
    mComponent->setData(mCode, QUrl{});
    qDebug() << mComponent->errorString();
  }
}

serial_protocol::~serial_protocol()
{
}
}
}
