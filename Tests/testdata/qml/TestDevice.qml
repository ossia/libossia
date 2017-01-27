import QtQuick 2.0
import org.ossia 1.0

Item {
    // /bar
    OssiaNode {
        id: tutu
        device: OssiaSingleDevice
        node: "bar"
    }

    // /bar/x
    OssiaPropertyBase on x {
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
        OssiaPropertyBase on rotation {
            device: OssiaSingleDevice
            node: "/foo/rot"
        }

        // /bar/rect_y
        OssiaPropertyBase on y {
            device: OssiaSingleDevice
            node: "rect_y"
        }

        // /bar/rect/width
        OssiaPropertyBase on width {
            device: OssiaSingleDevice
            node: "rect/width"
        }

    }
}
