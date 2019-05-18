import QtQuick 2.0
import Ossia 1.0 as Ossia

Ossia.Device
{
    property string address: "ws://127.0.0.1:5678"
    property int localPort: 10203

    onAddressChanged: openOSCQueryClient(address, localPort)
    onLocalPortChanged: openOSCQueryClient(address, localPort)
}
