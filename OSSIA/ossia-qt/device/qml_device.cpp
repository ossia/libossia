// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_device.hpp"
#include "qml_node_base.hpp"
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/detail/logger.hpp>
#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTimer>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/device/qml_parameter.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_property_reader.hpp>
#include <ossia-qt/device/qml_signal.hpp>
#include <ossia-qt/qml_context.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <ossia/network/common/debug.hpp>
#if defined(OSSIA_PROTOCOL_MIDI)
#include <ossia/network/midi/midi.hpp>
#endif
#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/network/osc/osc.hpp>
#endif
#include <QCoreApplication>
#include <wobjectimpl.h>

W_OBJECT_IMPL(ossia::qt::qml_device)
W_OBJECT_IMPL(ossia::qt::qml_singleton_device)
namespace ossia
{
namespace qt
{

qml_device::qml_device(QObject* parent)
    : QObject{parent}
    , m_device{std::make_unique<ossia::net::generic_device>(
          m_name.toUtf8().toStdString())}
{
}

net::device_base& qml_device::device()
{
  return *m_device;
}

const net::device_base& qml_device::device() const
{
  return *m_device;
}

net::multiplex_protocol* qml_device::localProtocol() const
{
  return dynamic_cast<ossia::net::multiplex_protocol*>(
      &device().get_protocol());
}

bool qml_device::readPreset() const
{
  return m_readPreset;
}

void qml_device::add(qml_node* n)
{
  m_nodes.insert({n, n});
}
void qml_device::remove(qml_node* n)
{
  m_nodes.erase(n);
}

void qml_device::add(qml_property* n)
{
  m_properties.insert({n, n});
}
void qml_device::remove(qml_property* n)
{
  m_properties.erase(n);
}

void qml_device::add(qml_parameter* n)
{
  m_parameters.insert({n, n});
}
void qml_device::remove(qml_parameter* n)
{
  m_parameters.erase(n);
}

void qml_device::add(qml_signal* n)
{
  m_signals.insert({n, n});
}
void qml_device::remove(qml_signal* n)
{
  m_signals.erase(n);
}

void qml_device::add(qml_property_reader* n)
{
  m_reader_properties.insert({n, n});
}
void qml_device::remove(qml_property_reader* n)
{
  m_reader_properties.erase(n);
}

void qml_device::add(qml_property_writer* n)
{
  m_writer_properties.insert({n, n});
}
void qml_device::remove(qml_property_writer* n)
{
  m_writer_properties.erase(n);
}

void qml_device::add(qml_model_property* n)
{
  m_models.insert({n, n});
}
void qml_device::remove(qml_model_property* n)
{
  m_models.erase(n);
}

void qml_device::add(qml_binding* n)
{
  m_bindings.insert({n, n});
}
void qml_device::remove(qml_binding* n)
{
  m_bindings.erase(n);
}

void qml_device::add(qml_callback* n)
{
  m_callbacks.insert({n, n});
}
void qml_device::remove(qml_callback* n)
{
  m_callbacks.erase(n);
}

QString qml_device::name() const
{
  return m_name;
}

void qml_device::setupLocal()
{
  // If there is an error we re-create a dummy device instead.
  m_device = std::make_unique<ossia::net::generic_device>(
      m_name.toUtf8().toStdString());
}

bool qml_device::openOSC(QString ip, int localPort, int remotePort)
{
  m_device.reset();

  try
  {
    auto proto = new ossia::net::osc_protocol{
        ip.toStdString(), (uint16_t)remotePort, (uint16_t)localPort};
    m_device = std::make_unique<ossia::net::generic_device>(
        std::unique_ptr<ossia::net::protocol_base>(proto),
        m_name.toUtf8().toStdString());
    return true;
  }
  catch (std::exception& e)
  {
    ossia::logger().error("qml_device::openOSC: {}", e.what());
  }
  catch (...)
  {
    ossia::logger().error("qml_device::openOSC: error");
  }

  setupLocal();
  return false;
}

bool qml_device::openOSCQueryServer(int wsPort, int oscPort)
{
  try
  {
    if (auto local = localProtocol())
    {
      auto& protos = local->get_protocols();
      while (!protos.empty())
        local->stop_expose_to(*protos.back());

      auto proto = std::make_unique<ossia::oscquery::oscquery_server_protocol>(
          oscPort, wsPort);
      // proto->set_logger(ossia::net::network_logger{spdlog::get("ossia"),
      // spdlog::get("ossia")});
      local->expose_to(std::move(proto));
      return true;
    }
  }
  catch (std::exception& e)
  {
    ossia::logger().error("qml_device::openOSCQueryServer: {}", e.what());
  }
  catch (...)
  {
    ossia::logger().error("qml_device::openOSCQueryServer: error");
  }
  return false;
}

bool qml_device::openOSCQueryClient(QString address, int localOscPort)
{
  m_device.reset();
  try
  {
    auto proto = new ossia::oscquery::oscquery_mirror_protocol{
        address.toStdString(), (uint16_t)localOscPort};
    m_device = std::make_unique<ossia::net::generic_device>(
        std::unique_ptr<ossia::net::protocol_base>(proto),
        m_name.toUtf8().toStdString());
    proto->update(m_device->get_root_node());
    return true;
  }
  catch (std::exception& e)
  {
    ossia::logger().error("qml_device::openOSCQueryClient: {}", e.what());
  }
  catch (...)
  {
    ossia::logger().error("qml_device::openOSCQueryClient: error");
  }

  setupLocal();
  return false;
}

bool qml_device::openMIDIInputDevice(int port)
{
  m_device.reset();

#if defined(OSSIA_PROTOCOL_MIDI)
  try
  {
    using namespace ossia::net::midi;
    auto proto = new midi_protocol{
        midi_info{midi_info::Type::RemoteOutput, {}, port}};
    auto dev = std::make_unique<midi_device>(
        std::unique_ptr<ossia::net::protocol_base>(proto));
    dev->update_namespace();
    m_device = std::move(dev);
    m_device->set_name(m_name.toUtf8().toStdString());
    return true;
  }
  catch (std::exception& e)
  {
    ossia::logger().error("qml_device::openMIDIInputDevice: {}", e.what());
  }
  catch (...)
  {
    ossia::logger().error("qml_device::openMIDIInputDevice: error");
  }
#endif

  setupLocal();
  return false;
}

bool qml_device::openMIDIOutputDevice(int port)
{
  m_device.reset();

#if defined(OSSIA_PROTOCOL_MIDI)
  try
  {
    using namespace ossia::net::midi;
    auto proto
        = new midi_protocol{midi_info{midi_info::Type::RemoteInput, {}, port}};
    auto dev = std::make_unique<midi_device>(
        std::unique_ptr<ossia::net::protocol_base>(proto));
    dev->update_namespace();
    m_device = std::move(dev);
    m_device->set_name(m_name.toUtf8().toStdString());

    return true;
  }
  catch (std::exception& e)
  {
    ossia::logger().error("qml_device::openMIDIOutputDevice: {}", e.what());
  }
  catch (...)
  {
    ossia::logger().error("qml_device::openMIDIOutputDevice: error");
  }
#endif

  setupLocal();
  return false;
}

QVariantMap qml_device::getMIDIInputDevices() const
{
  QVariantMap lst;

#if defined(OSSIA_PROTOCOL_MIDI)
  using namespace ossia::net::midi;
  midi_protocol p;
  for (const auto& info : p.scan())
  {
    if (info.type == midi_info::Type::RemoteOutput)
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
  for (const auto& info : p.scan())
  {
    if (info.type == midi_info::Type::RemoteInput)
      lst.insert(QString::fromStdString(info.device), info.port);
  }
#endif

  return lst;
}

void reset_items(QQuickItem* root)
{
  std::vector<QObject*> items;
  items.reserve(4096);
  for (auto cld : root->childItems())
  {
    items.push_back(cld);
  }

  for (auto cld : root->children())
  {
    items.push_back(cld);
  }

  std::size_t cur_pos = 0U;
  while (cur_pos < items.size())
  {
      if(auto qqi = qobject_cast<QQuickItem*>(items[cur_pos]))
      {
        for (auto cld : qqi->childItems())
          items.push_back(cld);
        for (auto cld : qqi->children())
          items.push_back(cld);
      }
      else
      {
        for (auto cld : items[cur_pos]->children())
          items.push_back(cld);
      }

    cur_pos++;
  }

  boost::container::flat_set<QObject*> objs(items.begin(), items.end());
  boost::range::remove_erase_if(items, [&objs] (auto ptr) {
      return objs.find(ptr) != objs.end();
  });

  for(auto ptr : items)
  {
    if (auto qn = qobject_cast<qml_node_base*>(ptr))
    {
      qn->resetNode();
      qApp->processEvents();
    }
  }
}

static bool processing_models = false;
void setup_models(std::size_t cur_model_size, std::size_t prev_model_size, qml_device& device) {

  auto mlist = device.models();
  for (auto model : mlist)
  {
    if (model.second)
    {
      qml_model_property* m = model.first;

      m->updateCount();
    }
    QCoreApplication::processEvents();
  }
  cur_model_size = device.models().size();

  if(cur_model_size != prev_model_size)
  {
    QTimer::singleShot(0, [=,&device] {
      setup_models(cur_model_size, prev_model_size, device);
    });
  }
  else
  {
    processing_models = false;
  }
}

void clear_models_preset(std::size_t cur_model_size, std::size_t prev_model_size, qml_device& device) {

  auto mlist = device.models();
  for (auto model : mlist)
  {
    if (model.second)
    {
      qml_model_property* m = model.first;

      m->setCount(0);
    }
    QCoreApplication::processEvents();
  }
  cur_model_size = device.models().size();

  if(cur_model_size != prev_model_size)
  {
    QTimer::singleShot(0, [=,&device] {
      clear_models_preset(cur_model_size, prev_model_size, device);
    });
  }
  else
  {
    device.setReadPreset(false);
    processing_models = false;
  }
}


void setup_models_preset(std::size_t cur_model_size, std::size_t prev_model_size, qml_device& device) {

  auto mlist = device.models();
  for (auto model : mlist)
  {
    if (model.second)
    {
      qml_model_property* m = model.first;

      m->updateCount();
    }
    QCoreApplication::processEvents();
  }
  cur_model_size = device.models().size();

  if(cur_model_size != prev_model_size)
  {
    QTimer::singleShot(0, [=,&device] {
      setup_models_preset(cur_model_size, prev_model_size, device);
    });
  }
  else
  {
    device.setReadPreset(false);
    processing_models = false;
  }
}

void qml_device::recreate(QObject* root)
{
  if (auto item = qobject_cast<QQuickItem*>(root))
  {
    reset_items(item);

    for (const auto& obj : m_nodes)
    {
      if (obj.second)
      {
        if(!obj.first->ossiaNode())
        {
          qDebug() << "recreate: Resetting: " << obj.first->node();
          obj.first->resetNode();
          qApp->processEvents();
        }
      }
      else
        this->remove(obj.first);
    }
    for_each_in_tuple(
        std::make_tuple(m_properties, m_parameters, m_signals),
        [this](auto& props) {
          for (const auto& obj : props)
          {
            if (obj.second)
            {
              obj.first->resetNode();
              qApp->processEvents();
            }
            else
              this->remove(obj.first);
          }
        });

    setup_models(m_models.size(), m_models.size(), *this);
    while(processing_models)
      qApp->processEvents();
    qApp->processEvents();
    qApp->processEvents();
    qApp->processEvents();
  }
  else
  {
    auto it = root->findChildren<QQuickItem*>("", Qt::FindDirectChildrenOnly);
    for(auto cld : it)
    {
      recreate(cld);
    }
  }
}

void qml_device::recreate_preset(QObject* root)
{
  if (auto item = qobject_cast<QQuickItem*>(root))
  {
    reset_items(item);
    for (const auto& obj : m_nodes)
    {
      if (obj.second)
      {
        if(!obj.first->ossiaNode())
        {
          qDebug() << "recreate_preset: Resetting: " << obj.first->node();
          obj.first->resetNode();
          qApp->processEvents();
        }
      }
      else
        this->remove(obj.first);
    }
    for_each_in_tuple(
        std::make_tuple(m_properties, m_parameters, m_signals),
        [this](auto& props) {
          for (const auto& obj : props)
          {
            if (obj.second)
            {
              obj.first->resetNode();
              qApp->processEvents();
            }
            else
              this->remove(obj.first);
          }
        });

    clear_models_preset(m_models.size(), m_models.size(), *this);
    while(processing_models)
      qApp->processEvents();
    //setup_models_preset(m_models.size(), m_models.size(), *this);
    while(processing_models)
      qApp->processEvents();
    qApp->processEvents();
    qApp->processEvents();
    qApp->processEvents();
  }
  else
  {
    auto it = root->findChildren<QQuickItem*>("", Qt::FindDirectChildrenOnly);
    for(auto cld : it)
    {
      recreate(cld);
    }
  }
}

void qml_device::remap(QObject* root)
{
  for_each_in_tuple(
      std::make_tuple(
          m_reader_properties, m_writer_properties, m_bindings, m_callbacks),
      [=](auto props) // Note: we voluntarily make a copy, due to erasing
                      // elements
      {
        for (auto obj : props)
        {
          if (obj.second)
            obj.first->resetNode();
          else
            this->remove(obj.first);
        }
      });
}

void qml_device::setReadPreset(bool readPreset)
{
  if (m_readPreset == readPreset)
    return;

  m_readPreset = readPreset;
  readPresetChanged(readPreset);
}

qml_device::~qml_device()
{
}

void qml_device::savePreset(const QUrl& file)
{
  try
  {
    if (file.isLocalFile())
    {
      QFile f(file.toLocalFile());
      if (f.open(QIODevice::WriteOnly))
      {
        auto preset = ossia::presets::make_preset(device().get_root_node());

        auto str = ossia::presets::write_json(device().get_name(), preset);
        f.write(str.data(), str.size());
        return;
      }
    }
  }
  catch (...)
  {
  }
  ossia::logger().error(
      "Could not save preset file: {}", file.toLocalFile().toStdString());
}

void qml_device::clearEmptyElements()
{
  for (auto it = m_properties.begin(); it != m_properties.end();)
    if (it->second)
      ++it;
    else
      it = m_properties.erase(it);

  for (auto it = m_parameters.begin(); it != m_parameters.end();)
    if (it->second)
      ++it;
    else
      it = m_parameters.erase(it);

  for (auto it = m_signals.begin(); it != m_signals.end();)
    if (it->second)
      ++it;
    else
      it = m_signals.erase(it);

  for (auto it = m_models.begin(); it != m_models.end();)
    if (it->second)
      ++it;
    else
      it = m_models.erase(it);
}
//#define PRESET_DEBUG 0
void qml_device::loadPreset(QObject* root, QString file)
{
  m_readPreset = false;
#if defined(PRESET_DEBUG)
  {
    // Before recreate
    fmt::MemoryWriter w;
    ossia::net::debug_recursively(w, this->device().get_root_node());
    qDebug() << "1:\n\n" << w.str().c_str();
  }
#endif

  recreate_preset(root);

#if defined(PRESET_DEBUG)
  {
    // After recreate
    fmt::MemoryWriter w;
    ossia::net::debug_recursively(w, this->device().get_root_node());
    qDebug() << "2:\n\n" << w.str().c_str();
  }
#endif
  try
  {
    QFile f;
    if (file.startsWith("file:"))
      f.setFileName(QUrl{file}.toLocalFile());
    else
      f.setFileName(file);

    if (f.open(QIODevice::ReadOnly))
    {

      m_readPreset = true;

      // Then load the preset
      auto kv = ossia::presets::read_json(f.readAll().toStdString(), false);
      ossia::presets::apply_preset(
          device().get_root_node(), kv, ossia::presets::keep_arch_off);

      // Clear empty elements that may have been removed
      clearEmptyElements();

#if defined(PRESET_DEBUG)
      {
        // After preset
        fmt::MemoryWriter w;
        ossia::net::debug_recursively(w, this->device().get_root_node());
        qDebug() << "3:\n\n" << w.str().c_str();
      }
#endif

      // Now as long as we are creating new models, update their count

      /*{
          std::size_t cur_model_size = m_models.size();
          std::size_t prev_model_size;
          do
          {
              prev_model_size = cur_model_size;
              auto mlist = m_models;
              for (auto model : mlist)
              {
                  if (model.second)
                  {
                      qml_model_property* m = model.first;

                      m->updateCount();
                  }
                  QCoreApplication::processEvents();
              }
              cur_model_size = m_models.size();
          } while (cur_model_size != prev_model_size);
      }
      */
      clearEmptyElements();

      // Finallt do a push of all properties registered
      recreate(root);

      clearEmptyElements();
      m_readPreset = false;
      return;
    }
  }
  catch (std::exception& e)
  {
    ossia::logger().error("{}", e.what());
  }
  catch (...)
  {
  }
  ossia::logger().error("Could not load preset file: {}", file.toStdString());
  m_readPreset = false;
}

void qml_device::saveDevice(const QUrl& file)
{
  try
  {
    if (file.isLocalFile())
    {
      {
        QFile f(file.toLocalFile());
        if (f.open(QIODevice::WriteOnly))
        {
          auto d = ossia::presets::write_json(device());
          f.write(d.data(), d.size());
          return;
        }
      }
    }
  }
  catch (...)
  {
  }
  ossia::logger().error(
      "Could not save device file: {}", file.toLocalFile().toStdString());
}

void qml_device::setName(QString name)
{
  if (m_name == name)
    return;

  m_name = name;
  if (m_device)
    m_device->set_name(name.toUtf8().toStdString());

  nameChanged(name);
}

void qml_device::cleanup()
{
  m_nodes.clear();
  m_properties.clear();
  m_parameters.clear();
  m_signals.clear();
  m_reader_properties.clear();
  m_writer_properties.clear();
  m_models.clear();
  m_bindings.clear();
  m_callbacks.clear();
  m_device->get_root_node().clear_children();
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
