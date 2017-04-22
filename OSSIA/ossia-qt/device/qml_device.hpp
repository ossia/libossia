#pragma once
#include <QObject>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <QPointer>
#include <QVariantMap>
#include <hopscotch_map.h>
namespace ossia
{
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

  Q_PROPERTY(QString appAuthor READ appAuthor WRITE setAppAuthor NOTIFY appAuthorChanged FINAL)
  Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged FINAL)
  Q_PROPERTY(QString appCreator READ appCreator WRITE setAppCreator NOTIFY appCreatorChanged FINAL)

public:
  qml_device(QObject* parent = nullptr);
  ~qml_device();

  ossia::net::device_base& device();
  const ossia::net::device_base& device() const;
  ossia::net::multiplex_protocol* localProtocol() const;

  bool readPreset() const;

  QString appAuthor() const;
  QString appVersion() const;
  QString appCreator() const;

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

public slots:
  void openOSC(QString ip, int localPort, int remotePort);
  void openOSCQueryServer(int WSport, int oscPort);
  void openOSCQueryClient(QString address, int localOscPort);
  void openMIDIInputDevice(int device);
  void openMIDIOutputDevice(int device);
  QVariantMap getMIDIInputDevices() const;
  QVariantMap getMIDIOutputDevices() const;

  void recreate(QObject* root);
  void remap(QObject* root);

  void setReadPreset(bool readPreset);

  void savePreset(const QUrl& file);
  void loadPreset(QObject* root, QString file);
  void saveDevice(const QUrl& file);

  void setAppAuthor(QString appAuthor);
  void setAppVersion(QString appVersion);
  void setAppCreator(QString appCreator);

signals:
  void readPresetChanged(bool readPreset);

  void appAuthorChanged(QString appAuthor);
  void appVersionChanged(QString appVersion);
  void appCreatorChanged(QString appCreator);

private:
  void setupLocal();
  void clearEmptyElements();

  std::unique_ptr<ossia::net::device_base> m_device;

  ptr_set<qml_node> m_nodes;
  ptr_set<qml_property> m_properties;
  ptr_set<qml_property_reader> m_reader_properties;
  ptr_set<qml_property_writer> m_writer_properties;
  ptr_set<qml_model_property> m_models;
  ptr_set<qml_binding> m_bindings;
  ptr_set<qml_callback> m_callbacks;

  QString m_appAuthor;
  QString m_appVersion;
  QString m_appCreator;
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
