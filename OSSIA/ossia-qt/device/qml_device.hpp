#pragma once
#include <QObject>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <QPointer>
#include <hopscotch_map.h>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_property;
class qml_model_property;
class qml_node_base;
class OSSIA_EXPORT qml_device :
    public QObject
{
  Q_OBJECT
  Q_PROPERTY(int wsPort READ wsPort WRITE setWSPort NOTIFY WSPortChanged FINAL)
  Q_PROPERTY(int oscPort READ oscPort WRITE setOSCPort NOTIFY OSCPortChanged FINAL)
  Q_PROPERTY(bool readPreset READ readPreset WRITE setReadPreset NOTIFY readPresetChanged FINAL)

  Q_PROPERTY(QString appAuthor READ appAuthor WRITE setAppAuthor NOTIFY appAuthorChanged FINAL)
  Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY appVersionChanged FINAL)
  Q_PROPERTY(QString appCreator READ appCreator WRITE setAppCreator NOTIFY appCreatorChanged FINAL)

public:
  qml_device(QObject* parent = nullptr);
  ~qml_device();
  void updateServer();

  ossia::net::generic_device& device();
  const ossia::net::generic_device& device() const;
  ossia::net::local_protocol& localProtocol() const;

  int wsPort() const;
  int oscPort() const;

  bool readPreset() const;

  tsl::hopscotch_map<qml_node*, QPointer<qml_node>> nodes;
  tsl::hopscotch_map<qml_property*, QPointer<qml_property>> properties;
  tsl::hopscotch_map<qml_model_property*, QPointer<qml_model_property>> models;

  QString appAuthor() const;

  QString appVersion() const;

  QString appCreator() const;

  public slots:
  void setWSPort(int wsPort);
  void setOSCPort(int oscPort);

  void rescan(QObject* root);

  void setReadPreset(bool readPreset);

  void savePreset(const QUrl& file);
  void loadPreset(QObject* root, QString file);
  void saveDevice(const QUrl& file);

  void setAppAuthor(QString appAuthor);

  void setAppVersion(QString appVersion);

  void setAppCreator(QString appCreator);

  signals:
  void WSPortChanged(int wsPort);
  void OSCPortChanged(int oscPort);

  void readPresetChanged(bool readPreset);

  void appAuthorChanged(QString appAuthor);

  void appVersionChanged(QString appVersion);

  void appCreatorChanged(QString appCreator);

  private:
  void clearEmptyElements();

  QString m_localName{"newDevice"};
  int m_wsPort{5678};
  int m_oscPort{9998};

  ossia::net::generic_device m_localDevice;
  bool m_readPreset{false};
  QString m_appAuthor;
  QString m_appVersion;
  QString m_appCreator;
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
