#include "qml_device.hpp"
#include "qml_node_base.hpp"
#include <QTimer>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <ossia-qt/qml_context.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_property_reader.hpp>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#if defined(OSSIA_PROTOCOL_MIDI)
#include <ossia/network/midi/midi.hpp>
#endif
#include <QCoreApplication>
namespace ossia
{
namespace qt
{

qml_device::qml_device(QObject* parent):
  QObject{parent},
  m_device{std::make_unique<ossia::net::generic_device>(std::make_unique<ossia::net::local_protocol>(), "device")}
{
}

void qml_device::openOSCQueryServer()
{
  try {
    if(auto local = localProtocol())
    {
      auto& protos = local->getExposedProtocols();
      while(!protos.empty())
        local->stopExposeTo(*protos.back());

      local->exposeTo(
            std::make_unique<ossia::oscquery::oscquery_server_protocol>(
              oscPort(),
              wsPort()));
    }
  } catch(...) { }
}

net::device_base&qml_device::device()
{ return *m_device; }

const net::device_base&qml_device::device() const
{ return *m_device; }

net::local_protocol* qml_device::localProtocol() const
{
  return dynamic_cast<ossia::net::local_protocol*>(&device().getProtocol());
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

QString qml_device::appAuthor() const
{
  return m_appAuthor;
}

QString qml_device::appVersion() const
{
  return m_appVersion;
}

QString qml_device::appCreator() const
{
  return m_appCreator;
}

void qml_device::setupLocal()
{
  // If there is an error we re-create a dummy device instead.
  m_device = std::make_unique<ossia::net::generic_device>(std::make_unique<ossia::net::local_protocol>(), "device");
}

void qml_device::openOSCQueryClient(QString address, int port)
{
  try {
    auto proto = new ossia::oscquery::oscquery_mirror_protocol{address.toStdString(), (unsigned int)port};
    m_device = std::make_unique<ossia::net::generic_device>(std::unique_ptr<ossia::net::protocol_base>(proto), "device");
    proto->update(m_device->getRootNode());
    return;
  } catch(std::exception& e) {
    ossia::logger().error("qml_device::openOSCQueryClient: {}", e.what());
  } catch(...) {
    ossia::logger().error("qml_device::openOSCQueryClient: error");
  }

  setupLocal();
}

void qml_device::openMIDIInputDevice(int port)
{
#if defined(OSSIA_PROTOCOL_MIDI)
  try {
    using namespace ossia::net::midi;
    auto proto = new midi_protocol{midi_info{midi_info::Type::RemoteOutput, {}, (unsigned int)port}};
    auto dev = std::make_unique<midi_device>(std::unique_ptr<ossia::net::protocol_base>(proto));
    dev->updateNamespace();
    m_device = std::move(dev);
    return;
  } catch(std::exception& e) {
    ossia::logger().error("qml_device::openMIDIInputDevice: {}", e.what());
  } catch(...) {
    ossia::logger().error("qml_device::openMIDIInputDevice: error");
  }
#endif

  setupLocal();
}

void qml_device::openMIDIOutputDevice(int port)
{
#if defined(OSSIA_PROTOCOL_MIDI)
  try {
    using namespace ossia::net::midi;
    auto proto = new midi_protocol{midi_info{midi_info::Type::RemoteInput, {}, port}};
    auto dev = std::make_unique<midi_device>(std::unique_ptr<ossia::net::protocol_base>(proto));
    dev->updateNamespace();
    m_device = std::move(dev);

    return;
  } catch(std::exception& e) {
    ossia::logger().error("qml_device::openMIDIOutputDevice: {}", e.what());
  } catch(...) {
    ossia::logger().error("qml_device::openMIDIOutputDevice: error");
  }
#endif

  setupLocal();
}

QVariantMap qml_device::getMIDIInputDevices() const
{
  QVariantMap lst;

#if defined(OSSIA_PROTOCOL_MIDI)
  using namespace ossia::net::midi;
  midi_protocol p;
  for(const auto& info : p.scan())
  {
    if(info.type == midi_info::Type::RemoteOutput)
      lst.insert(QString::fromStdString(info.device), info.port);
  }
#endif

  return lst;
}

QVariantMap qml_device::getMIDIOutputDevices() const
{
  QVariantMap lst;

#if defined(OSSIA_PROTOCOL_MIDI)
  using namespace ossia::net::midi;
  midi_protocol p;
  for(const auto& info : p.scan())
  {
    if(info.type == midi_info::Type::RemoteInput)
      lst.insert(QString::fromStdString(info.device), info.port);
  }
#endif

  return lst;
}

void qml_device::setWSPort(int localPort)
{
  if (m_wsPort == localPort)
    return;

  m_wsPort = localPort;
  emit WSPortChanged(localPort);
}

void qml_device::setOSCPort(int remotePort)
{
  if (m_oscPort == remotePort)
    return;

  m_oscPort = remotePort;
  emit OSCPortChanged(remotePort);
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

    {
      auto props = properties;
      for(auto obj : props)
      {
        if(obj.second) obj.first->resetNode();
        else properties.erase(obj.first);
      }
    }

    //for(auto obj : models)
    {
      // TODO ?
    }
  }
}

void qml_device::remap(QObject* root)
{
  auto props = reader_properties;
  for(auto obj : props)
  {
    if(obj.second) obj.first->resetNode();
    else reader_properties.erase(obj.first);
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
  try {
    if(file.isLocalFile())
    {
      QFile f(file.toLocalFile());
      if(f.open(QIODevice::WriteOnly))
      {
        auto preset = ossia::devices::make_preset(device());

        auto str = ossia::presets::write_json(preset);
        f.write(str.data(), str.size());
        return;
      }
    }
  } catch(...) {

  }
  ossia::logger().error("Could not save preset file: {}", file.toLocalFile().toStdString());
}

void qml_device::clearEmptyElements()
{
  for(auto it = properties.begin(); it != properties.end();)
    if(it->second) ++it; else it = properties.erase(it);

  for(auto it = models.begin(); it != models.end();)
    if(it->second) ++it; else it = models.erase(it);

}

void qml_device::loadPreset(QObject* root, QString file)
{
  m_readPreset = false;
  rescan(root);
  try {
    QFile f(file);
    if(f.open(QIODevice::ReadOnly))
    {
      // First reset all item models since they will be in the preset
      {
        auto model_list = models;
        for(auto model : model_list)
        {
          if(model.second) model.first->setCount(0);
          else models.erase(model.first);
        }
      }
      m_readPreset = true;

      // Then load the preset
      auto kv = ossia::presets::read_json(f.readAll().toStdString());
      ossia::devices::apply_preset(device(), kv, ossia::devices::keep_arch_off);

      // Clear empty elements that may have been removed
      clearEmptyElements();

      // Now as long as we are creating new models, update their count
      std::size_t cur_model_size = models.size();
      std::size_t prev_model_size;
      do {
        prev_model_size = cur_model_size;
        auto mlist = models;
        for(auto model : mlist)
        {
          if(model.second) model.first->updateCount();
          QCoreApplication::processEvents();
        }
        cur_model_size = models.size();
      } while(cur_model_size != prev_model_size);

      clearEmptyElements();

      // Finallt do a push of all properties registered
      rescan(root);
      return;
    }
  } catch(std::exception& e) { ossia::logger().error("{}", e.what());
                             } catch(...) { }
  ossia::logger().error("Could not load preset file: {}", file.toStdString());
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
          return;
        }
      }
    }
  } catch(...) {
  }
  ossia::logger().error("Could not save device file: {}", file.toLocalFile().toStdString());
}

void qml_device::setAppAuthor(QString appAuthor)
{
  if (m_appAuthor == appAuthor)
    return;

  m_appAuthor = appAuthor;
  emit appAuthorChanged(appAuthor);
}

void qml_device::setAppVersion(QString appVersion)
{
  if (m_appVersion == appVersion)
    return;

  m_appVersion = appVersion;
  emit appVersionChanged(appVersion);
}

void qml_device::setAppCreator(QString appCreator)
{
  if (m_appCreator == appCreator)
    return;

  m_appCreator = appCreator;
  emit appCreatorChanged(appCreator);
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
