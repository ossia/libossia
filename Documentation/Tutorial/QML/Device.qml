import QtQuick 2.0
import Ossia 1.0 as Ossia

Item
{
    id: root
    //// ~ Welcome to the libossia QML tutorial! ~ ////

    /////////////////////////////////////////////////////
    //// Step 1. Creating a device with a few nodes. ////
    /////////////////////////////////////////////////////
    // QML is a declarative & reactive language,
    // mainly used for user interfaces.
    // The Ossia primitives are objects in QML, whose organization
    // in the code directly reflects the ossia device tree.

    // Create a device
    Ossia.OSCQueryServer {
        id: dev
        name: "supersoftware"
    }

    // A QML object
    Rectangle {
        width: 50
        height: 50
        Ossia.Node { node: "foo" }

        // /foo/color, type ARGB
        Ossia.Property on color { }

        // /foo/baz, type float
        Ossia.Property on scale {
            node: "baz"
            min: -5
            max: 5
        }
    }

    ////////////////////////////////////////////////////////////////////////
    //// Step 2. Creating another device to connect with the first one. ////
    ////////////////////////////////////////////////////////////////////////
    Ossia.OSCQueryClient {
        id: remote_dev
        name: "supersoftware"
        address: "ws://127.0.0.1:5678"
    }

    // When the scale of the rectangle changes,
    // onValueChanged is called here:
    Ossia.Callback {
        device: remote_dev
        node: "/foo/baz"
        onValueChanged: console.log(value)
    }

    Component.onCompleted: {
        dev.recreate(root)
        remote_dev.remap(root)
    }
}
