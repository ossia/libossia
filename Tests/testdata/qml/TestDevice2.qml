import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import Ossia 1.0 as Ossia

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Ossia.Property on width {}

    Row
    {
        Repeater {
            model: Ossia.Instances { node: "leText"; count: 5 }
            delegate: Text {
                Ossia.Node { node: "leText." + index }
                Ossia.Property on text { }
                Ossia.Property on font.pointSize { }
                Ossia.Property on color { }
                text: "" + index
            }
        }
    }

    Component.onCompleted:
    {
    //    Ossia.savePreset(OssiaSingleDevice, "file:///tmp/preset.json")
        Ossia.SingleDevice.setReadPreset(true);
        Ossia.SingleDevice.loadPreset("file:///tmp/preset.json")
    }
}
