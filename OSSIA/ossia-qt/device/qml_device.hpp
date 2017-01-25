#pragma once
#include <QObject>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>

namespace ossia
{
namespace qt
{
class qml_device :
        public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString localName READ localName WRITE setLocalName NOTIFY localNameChanged)
    Q_PROPERTY(QString remoteName READ remoteName WRITE setRemoteName NOTIFY remoteNameChanged)
    Q_PROPERTY(QString remoteIp READ remoteIp WRITE setRemoteIp NOTIFY remoteIpChanged)
    Q_PROPERTY(int localPort READ localPort WRITE setLocalPort NOTIFY localPortChanged)
    Q_PROPERTY(int remotePort READ remotePort WRITE setRemotePort NOTIFY remotePortChanged)

public:
    qml_device(QObject* parent = nullptr);
    ~qml_device();
    void updateMinuit();

    ossia::net::generic_device& device();
    const ossia::net::generic_device& device() const;

    ossia::net::local_protocol& localProtocol() const;

    QString localName() const;
    QString remoteName() const;
    int localPort() const;
    int remotePort() const;
    QString remoteIp() const;

public slots:
    void setLocalName(QString localName);
    void setRemoteName(QString remoteName);
    void setLocalPort(int localPort);
    void setRemotePort(int remotePort);
    void setRemoteIp(QString remoteIp);

signals:
    void localNameChanged(QString localName);
    void remoteNameChanged(QString remoteName);
    void localPortChanged(int localPort);
    void remotePortChanged(int remotePort);
    void remoteIpChanged(QString remoteIp);

private:
    QString m_localName{"newDevice"};
    QString m_remoteName{"i-score"};
    int m_localPort{9998};
    int m_remotePort{13579};

    ossia::net::generic_device m_localDevice;
    QString m_remoteIp;
};

class qml_singleton_device : public qml_device
{

};
}
}
