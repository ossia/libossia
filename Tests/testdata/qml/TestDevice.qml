import QtQuick 2.0
import Ossia 1.0 as Ossia

Item {
    // /bar
    Ossia.Node {
        id: tutu
        device: OssiaSingleDevice
        node: "bar"
    }

    // /bar/x
    Ossia.Property on x {
        device: OssiaSingleDevice
    }

    PropertyAnimation on x {
        duration: 1000
        loops: Animation.Infinite
        from: 0
        to: 100
    }
    Rectangle {
        x: 100
        y: 100
        width: 100
        height: 100
        color: "black"

        // /foo/rot
        Ossia.Property on rotation {
            device: OssiaSingleDevice
            node: "/foo/rot"
        }

        // /bar/rect_y
        Ossia.Property on y {
            device: OssiaSingleDevice
            node: "rect_y"
        }

        // /bar/rect/width
        Ossia.Property on width {
            device: OssiaSingleDevice
            node: "rect/width"
        }

    }
}
