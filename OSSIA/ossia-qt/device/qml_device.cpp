#include "qml_device.hpp"
#include "qml_node_base.hpp"
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <ossia-qt/qml_context.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia/network/common/debug.hpp>
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

bool qml_device::readPreset() const
{
  return m_readPreset;
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

std::vector<QQuickItem*> items(QQuickItem* root)
{
  std::vector<QQuickItem*> items;
  items.reserve(4096);
  for(auto cld : root->childItems())
  {
    items.push_back(cld);
  }

  std::size_t cur_pos = 0U;
  while(cur_pos < items.size())
  {
    for(auto cld : items[cur_pos]->childItems())
      items.push_back(cld);

    cur_pos++;
  }

  return items;
}

void qml_device::rescan(QObject* root)
{
    if(auto item = qobject_cast<QQuickItem*>(root))
    {
      for(auto cld : items(item))
      {
        if(auto qn = qobject_cast<qml_node_base*>(cld))
        {
          qn->resetNode();
        }
      }

      for(auto obj : properties)
      {
        obj->resetNode();
      }
    }
}

void qml_device::setReadPreset(bool readPreset)
{
  if (m_readPreset == readPreset)
    return;

  m_readPreset = readPreset;
  emit readPresetChanged(readPreset);
}

qml_device::~qml_device()
{

}

void qml_device::savePreset(const QUrl& file)
{
  // fmt::MemoryWriter w;
  // ossia::net::debug_recursively(w, m_localDevice.getRootNode());
  // std::cerr << w.str();
  try {
    if(file.isLocalFile())
    {
      QFile f(file.toLocalFile());
      if(f.open(QIODevice::WriteOnly))
      {
        auto preset = ossia::devices::make_preset(device());

        // for(auto e : preset)
        //  std::cerr << e.first << e.second << std::endl;

        auto str = ossia::presets::write_json(preset);
        f.write(str.data(), str.size());
      }
    }
  } catch(...) {
    ossia::logger().error("Could not save preset file: {}", file.toLocalFile().toStdString());
  }
}

void qml_device::loadPreset(const QUrl& file)
{
  try {
    if(file.isLocalFile())
    {
      QFile f(file.toLocalFile());
      if(f.open(QIODevice::ReadOnly))
      {
        auto kv = ossia::presets::read_json(f.readAll().toStdString());
        ossia::devices::apply_preset(device(), kv);
        return;
      }
    }
  } catch(...) {
    ossia::logger().error("Could not load preset file: {}", file.toLocalFile().toStdString());
  }
}

void qml_device::saveDevice(const QUrl& file)
{
  try {
    if(file.isLocalFile())
    {
      {
        QFile f(file.toLocalFile());
        if(f.open(QIODevice::WriteOnly))
        {
          auto d = ossia::devices::write_json(device());
          f.write(d.data(), d.size());
        }
      }
    }
  } catch(...) {
    ossia::logger().error("Could not save device file: {}", file.toLocalFile().toStdString());
  }
}

qml_singleton_device::qml_singleton_device()
{
  QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

qml_singleton_device::~qml_singleton_device()
{

}

qml_singleton_device& qml_singleton_device::instance()
{
  static qml_singleton_device dev;
  return dev;
}



}
}
