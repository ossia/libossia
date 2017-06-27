#pragma once
#include <QObject>
#include <QPointer>
#include <QVariantMap>
#include <hopscotch_map.h>
#include <ossia_export.h>
namespace ossia
{
namespace net { class device_base; }
namespace net { class generic_device; }
namespace net { class multiplex_protocol; }
namespace qt
{

template<typename T>
using ptr_set = tsl::hopscotch_map<T*, QPointer<T>>;

class qml_node;
class qml_property;
class qml_property_reader;
class qml_property_writer;
class qml_model_property;
class qml_node_base;
class qml_binding;
class qml_callback;
class OSSIA_EXPORT qml_device :
    public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool readPreset READ readPreset WRITE setReadPreset NOTIFY readPresetChanged FINAL)
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

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

public slots:
  bool openOSC(QString ip, int localPort, int remotePort);
  bool openOSCQueryServer(int WSport, int oscPort);
  bool openOSCQueryClient(QString address, int localOscPort);
  bool openMIDIInputDevice(int device);
  bool openMIDIOutputDevice(int device);
  QVariantMap getMIDIInputDevices() const;
  QVariantMap getMIDIOutputDevices() const;

  void recreate(QObject* root);
  void remap(QObject* root);

  void setReadPreset(bool readPreset);

  void savePreset(const QUrl& file);
  void loadPreset(QObject* root, QString file);
  void saveDevice(const QUrl& file);

  void setName(QString name);

signals:
  void readPresetChanged(bool readPreset);
  void nameChanged(QString name);

private:
  void setupLocal();
  void clearEmptyElements();

  QString m_name{"device"};
  std::unique_ptr<ossia::net::device_base> m_device;

  ptr_set<qml_node> m_nodes;
  ptr_set<qml_property> m_properties;
  ptr_set<qml_property_reader> m_reader_properties;
  ptr_set<qml_property_writer> m_writer_properties;
  ptr_set<qml_model_property> m_models;
  ptr_set<qml_binding> m_bindings;
  ptr_set<qml_callback> m_callbacks;
  bool m_readPreset{false};
};


class OSSIA_EXPORT qml_singleton_device : public qml_device
{
  Q_OBJECT
public:
  static qml_singleton_device& instance();

private:
  qml_singleton_device();
  ~qml_singleton_device();
};
}
}
