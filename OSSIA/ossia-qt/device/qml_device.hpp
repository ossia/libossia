#pragma once
#include <QObject>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <hopscotch_set.h>
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
    Q_PROPERTY(int wsPort READ wsPort WRITE setWSPort NOTIFY WSPortChanged)
    Q_PROPERTY(int oscPort READ oscPort WRITE setOSCPort NOTIFY OSCPortChanged)
    Q_PROPERTY(bool readPreset READ readPreset WRITE setReadPreset NOTIFY readPresetChanged)

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

    tsl::hopscotch_set<qml_node*> nodes;
    tsl::hopscotch_set<qml_property*> properties;
    tsl::hopscotch_set<qml_model_property*> models;

public slots:
    void setWSPort(int wsPort);
    void setOSCPort(int oscPort);

    void rescan(QObject* root);

    void setReadPreset(bool readPreset);

    void savePreset(const QUrl& file);
    void loadPreset(const QUrl& file);
    void saveDevice(const QUrl& file);

signals:
    void WSPortChanged(int wsPort);
    void OSCPortChanged(int oscPort);

    void readPresetChanged(bool readPreset);

private:
    QString m_localName{"newDevice"};
    int m_wsPort{5678};
    int m_oscPort{9998};

    ossia::net::generic_device m_localDevice;
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
