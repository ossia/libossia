import QtQuick 2.0
import Ossia 1.0 as Ossia

Ossia.Device
{
    property string address
    onAddressChanged: openOSCQueryClient(address, wsPort)
}
