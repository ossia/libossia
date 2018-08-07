import QtQuick 2.0
import Ossia 1.0 as Ossia
import "Ossia" as Ossia

Item {
    Ossia.OSCQueryClient
    {
        id: theDevice
        address: "ws://127.0.0.1:5678"
    }

    // /bar
    Ossia.Callback {
        id: tutu
        device: theDevice
        node: "/test/my_float"
        onValueChanged: console.log(value)
    }

    id: root
    Component.onCompleted: {
        theDevice.openOSCQueryClient(theDevice.address, theDevice.localPort)
        theDevice.remap(root)
    }
}
