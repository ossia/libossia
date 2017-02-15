#pragma once
#include <QObject>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace qt
{
class OSSIA_EXPORT qml_device :
        public QObject
{
    Q_OBJECT
    Q_PROPERTY(int wsPort READ wsPort WRITE setWSPort NOTIFY WSPortChanged)
    Q_PROPERTY(int oscPort READ oscPort WRITE setOSCPort NOTIFY OSCPortChanged)

public:
    qml_device(QObject* parent = nullptr);
    ~qml_device();
    void updateServer();

    ossia::net::generic_device& device();
    const ossia::net::generic_device& device() const;
    ossia::net::local_protocol& localProtocol() const;

    int wsPort() const;
    int oscPort() const;

public slots:
    void setWSPort(int wsPort);
    void setOSCPort(int oscPort);

signals:
    void WSPortChanged(int wsPort);
    void OSCPortChanged(int oscPort);

private:
    QString m_localName{"newDevice"};
    int m_wsPort{5678};
    int m_oscPort{9998};

    ossia::net::generic_device m_localDevice;
};

class OSSIA_EXPORT qml_singleton_device : public qml_device
{
  Q_OBJECT
public:
  static qml_singleton_device& instance();

private:
  qml_singleton_device();
};
}
}
