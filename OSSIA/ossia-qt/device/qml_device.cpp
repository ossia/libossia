#include "qml_device.hpp"

namespace ossia
{
namespace qt
{

qml_device::qml_device(QObject* parent):
    QObject{parent},
    m_localDevice{std::make_unique<ossia::net::local_protocol>(), localName().toStdString()}
{

}

void qml_device::updateMinuit()
{
    try {
        auto& protos = localProtocol().getExposedProtocols();
        while(!protos.empty())
            localProtocol().stopExposeTo(*protos.back());

        localProtocol().exposeTo(
                    std::make_unique<ossia::net::minuit_protocol>(
                        localName().toStdString(),
                        remoteIp().toStdString(),
                        remotePort(),
                        localPort()));
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

QString qml_device::localName() const
{
    return m_localName;
}

QString qml_device::remoteName() const
{
    return m_remoteName;
}

int qml_device::localPort() const
{
    return m_localPort;
}

int qml_device::remotePort() const
{
    return m_remotePort;
}

QString qml_device::remoteIp() const
{
    return m_remoteIp;
}

void qml_device::setLocalName(QString localName)
{
    if (m_localName == localName)
        return;

    m_localName = localName;
    emit localNameChanged(localName);
    updateMinuit();
}

void qml_device::setRemoteName(QString remoteName)
{
    if (m_remoteName == remoteName)
        return;

    m_remoteName = remoteName;
    emit remoteNameChanged(remoteName);
    updateMinuit();
}

void qml_device::setLocalPort(int localPort)
{
    if (m_localPort == localPort)
        return;

    m_localPort = localPort;
    emit localPortChanged(localPort);
    updateMinuit();
}

void qml_device::setRemotePort(int remotePort)
{
    if (m_remotePort == remotePort)
        return;

    m_remotePort = remotePort;
    emit remotePortChanged(remotePort);
    updateMinuit();
}

void qml_device::setRemoteIp(QString remoteIp)
{
    if (m_remoteIp == remoteIp)
        return;

    m_remoteIp = remoteIp;
    emit remoteIpChanged(remoteIp);
    updateMinuit();
}

qml_device::~qml_device()
{

}

}
}
