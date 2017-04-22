import Ossia 1.0 as Ossia

Ossia.Device
{
    property string ip: "127.0.0.1"
    property int localPort: 9998
    property int remotePort: 9999

    onIpChanged: openOSC(ip, localPort, remotePort)
    onLocalPortChanged: openOSC(ip, localPort, remotePort)
    onRemotePortChanged: openOSC(ip, localPort, remotePort)
}
