// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_SERIAL)

#include "serial_protocol.hpp"
#include <QDebug>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <iomanip>
#include <sstream>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::net::serial_wrapper)
namespace ossia
{
namespace net
{

serial_wrapper::~serial_wrapper()
{

}

serial_protocol::serial_protocol(
    const QByteArray& code, const QSerialPortInfo& bot)
    : m_engine{new QQmlEngine}
    , m_component{new QQmlComponent{m_engine}}
    , m_serialPort{bot}
    , m_code{code}
{
  connect(
      m_component, &QQmlComponent::statusChanged, this,
      [=](QQmlComponent::Status status) {
        qDebug() << status;
        qDebug() << m_component->errorString();
        if (!m_device)
          return;

        switch (status)
        {
          case QQmlComponent::Status::Ready:
          {
            m_object = m_component->create();
            m_object->setParent(m_engine->rootContext());

            QVariant ret;
            QMetaObject::invokeMethod(
                m_object, "createTree", Q_RETURN_ARG(QVariant, ret));
            qt::create_device<ossia::net::device_base, serial_node, serial_protocol>(
                *m_device, ret.value<QJSValue>());

            connect(&m_serialPort, &serial_wrapper::read,
                    this, [&] (const QByteArray& a) {

              QVariant ret;
              QMetaObject::invokeMethod(
                           m_object, "onMessage",
                           Q_RETURN_ARG(QVariant, ret),
                           Q_ARG(QVariant, QString::fromUtf8(a)));

              auto arr = ret.value<QJSValue>();
              // should be an array of { address, value } objects
              if (!arr.isArray())
                return;

              QJSValueIterator it(arr);
              while (it.hasNext())
              {
                it.next();
                auto val = it.value();
                auto addr = val.property("address");
                if (!addr.isString())
                  continue;

                auto addr_txt = addr.toString().toStdString();
                auto n = find_node(m_device->get_root_node(), addr_txt);
                if (!n)
                  continue;

                auto v = val.property("value");
                if (v.isNull())
                  continue;

                if (auto addr = n->get_parameter())
                {
                  qDebug() << "Applied value"
                           << QString::fromStdString(value_to_pretty_string(
                                  qt::value_from_js(addr->value(), v)));
                  addr->push_value(qt::value_from_js(addr->value(), v));
                }
              }
            });
            return;
          }
          case QQmlComponent::Status::Loading:
            return;
          case QQmlComponent::Status::Null:
          case QQmlComponent::Status::Error:
            qDebug() << m_component->errorString();
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
    case ossia::val_type::STRING:
      str.replace("$val", QString::fromStdString(ad.getValue().get<std::string>()));
      break;
    case ossia::val_type::IMPULSE:
      break;
    default:
      throw std::runtime_error("serial_protocol::push: bad type");
  }

  str += '\n';
  qDebug() << str;
  m_serialPort.write(str.toUtf8());
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
  m_device = &dev;
  m_component->setData(m_code, QUrl{});
}

serial_protocol::~serial_protocol()
{
}
}
}
#endif
