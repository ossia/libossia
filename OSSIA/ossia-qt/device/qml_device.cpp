#include "qml_device.hpp"
#include "qml_node_base.hpp"
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
namespace ossia
{
namespace qt
{

qml_device::qml_device(QObject* parent):
  QObject{parent},
  m_localDevice{std::make_unique<ossia::net::local_protocol>(), "device"}
{
  updateServer();
}

void qml_device::updateServer()
{
  try {
    auto& protos = localProtocol().getExposedProtocols();
    while(!protos.empty())
      localProtocol().stopExposeTo(*protos.back());

    localProtocol().exposeTo(
          std::make_unique<ossia::oscquery::oscquery_server_protocol>(
            oscPort(),
            wsPort()));
  } catch(...) { }
}

net::generic_device&qml_device::device()
{ return m_localDevice; }

const net::generic_device&qml_device::device() const
{ return m_localDevice; }

net::local_protocol&qml_device::localProtocol() const
{
  return static_cast<ossia::net::local_protocol&>(m_localDevice.getProtocol());
}

int qml_device::wsPort() const
{
  return m_wsPort;
}

int qml_device::oscPort() const
{
  return m_oscPort;
}

void qml_device::setWSPort(int localPort)
{
  if (m_wsPort == localPort)
    return;

  m_wsPort = localPort;
  emit WSPortChanged(localPort);
  updateServer();
}

void qml_device::setOSCPort(int remotePort)
{
  if (m_oscPort == remotePort)
    return;

  m_oscPort = remotePort;
  emit OSCPortChanged(remotePort);
  updateServer();
}

void qml_device::rescan(QObject* root)
{
  QTimer::singleShot(10, [=] {
  auto root_objects = root->parent()->findChildren<qml_node_base*>();

  for(auto cld : root_objects)
  {
    qDebug() << "resetting" << cld << cld->node() << cld->path();
    cld->resetNode();
  }
  });
}

qml_device::~qml_device()
{

}

qml_singleton_device::qml_singleton_device()
{
  QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

qml_singleton_device& qml_singleton_device::instance()
{
  static qml_singleton_device dev;
  return dev;
}

}
}
