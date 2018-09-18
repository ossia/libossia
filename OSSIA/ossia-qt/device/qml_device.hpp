#pragma once
#include <ossia/detail/config.hpp>
#include <QObject>
#include <QPointer>
#include <QVariantMap>
#include <tsl/hopscotch_map.h>
#include <wobjectdefs.h>
namespace ossia
{
namespace net
{
class device_base;
}
namespace net
{
class generic_device;
}
namespace net
{
class multiplex_protocol;
}
namespace qt
{

template <typename T>
using qpointer_set = tsl::hopscotch_map<T*, QPointer<T>>;

class qml_node;
class qml_parameter;
class qml_property;
class qml_signal;
class qml_property_reader;
class qml_property_writer;
class qml_model_property;
class qml_node_base;
class qml_binding;
class qml_callback;
class OSSIA_EXPORT qml_device : public QObject
{
  W_OBJECT(qml_device)
public:
  qml_device(QObject* parent = nullptr);
  ~qml_device();

  ossia::net::device_base& device();
  const ossia::net::device_base& device() const;
  ossia::net::multiplex_protocol* localProtocol() const;

  bool readPreset() const;

  void add(qml_node* n);
  void remove(qml_node* n);
  void add(qml_property* n);
  void remove(qml_property* n);
  void add(qml_parameter* n);
  void remove(qml_parameter* n);
  void add(qml_signal* n);
  void remove(qml_signal* n);

  void add(qml_property_reader* n);
  void remove(qml_property_reader* n);
  void add(qml_property_writer* n);
  void remove(qml_property_writer* n);
  void add(qml_model_property* n);
  void remove(qml_model_property* n);
  void add(qml_binding* n);
  void remove(qml_binding* n);
  void add(qml_callback* n);
  void remove(qml_callback* n);

  QString name() const;

  const qpointer_set<qml_model_property>& models() const { return m_models; }

  bool openOSC(const QString& ip, int localPort, int remotePort); W_INVOKABLE(openOSC)
  bool openOSCQueryServer(int WSport, int oscPort); W_INVOKABLE(openOSCQueryServer)
  bool openOSCQueryClient(const QString& address, int localOscPort); W_INVOKABLE(openOSCQueryClient)
  bool openMIDIInputDevice(int device); W_INVOKABLE(openMIDIInputDevice)
  bool openMIDIOutputDevice(int device); W_INVOKABLE(openMIDIOutputDevice)
  QVariantMap getMIDIInputDevices() const; W_INVOKABLE(getMIDIInputDevices)
  QVariantMap getMIDIOutputDevices() const; W_INVOKABLE(getMIDIOutputDevices)

  void recreate(QObject* root); W_INVOKABLE(recreate)
  void remap(QObject* root); W_INVOKABLE(remap)

  void setReadPreset(bool readPreset); W_INVOKABLE(setReadPreset)

  void savePreset(const QUrl& file); W_INVOKABLE(savePreset)
  void loadPreset(QObject* root, const QString& file); W_INVOKABLE(loadPreset)
  void saveDevice(const QUrl& file); W_INVOKABLE(saveDevice)

  void setName(QString name); W_INVOKABLE(setName)

  void cleanup(); W_INVOKABLE(cleanup)


  void readPresetChanged(bool readPreset)
  E_SIGNAL(OSSIA_EXPORT, readPresetChanged, readPreset)

  void presetFinishedLoading()
  E_SIGNAL(OSSIA_EXPORT, presetFinishedLoading)
  void presetFailedLoading(QString err)
  E_SIGNAL(OSSIA_EXPORT, presetFailedLoading, err)

  void nameChanged(QString name)
  E_SIGNAL(OSSIA_EXPORT, nameChanged, name)

  W_PROPERTY(bool, readPreset READ readPreset WRITE setReadPreset NOTIFY readPresetChanged, W_Final)
  W_PROPERTY(QString, name READ name WRITE setName NOTIFY nameChanged, W_Final)

private:
  void setupLocal();
  void clearEmptyElements();

  QString m_name{"device"};
  std::unique_ptr<ossia::net::device_base> m_device;

  qpointer_set<qml_node> m_nodes;
  qpointer_set<qml_property> m_properties;
  qpointer_set<qml_parameter> m_parameters;
  qpointer_set<qml_signal> m_signals;
  qpointer_set<qml_property_reader> m_reader_properties;
  qpointer_set<qml_property_writer> m_writer_properties;
  qpointer_set<qml_model_property> m_models;
  qpointer_set<qml_binding> m_bindings;
  qpointer_set<qml_callback> m_callbacks;
  bool m_readPreset{false};
  bool m_loadingPreset{false};
  void recreate_preset(QObject* root);
};

class OSSIA_EXPORT qml_singleton_device : public qml_device
{
  W_OBJECT(qml_singleton_device)
public:
  static qml_singleton_device& instance();

private:
  qml_singleton_device();
  ~qml_singleton_device();
};
}
}
W_REGISTER_ARGTYPE(ossia::qt::qml_device*)
