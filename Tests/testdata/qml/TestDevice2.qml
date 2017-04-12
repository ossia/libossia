import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import Ossia 1.0 as Ossia

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")


    Item {
        id: root
        anchors.fill: parent
        Ossia.Node { node: "/"; id: rootnode }
        Ossia.Property on height {}
        Column
        {
            Repeater {
                model: Ossia.Instances {
                    node: "leRect";
                    count: 5;
                    parentNode: rootnode;
                    onCountChanged: console.log("rect count: " + count)
                }

                delegate: Rectangle {
                    width: 800
                    height: 70
                    color: Qt.hsla(0.8 / (1+index), 1/ (1+index), 0.7 , 1)

                    Ossia.Node { node: "leRect." + index; id: rectnode }
                    Ossia.Property on border.width { node: "border_width"; parentNode: rectnode}
                    Text { Ossia.Property on font.pointSize {} }

                    Row
                    {
                        Repeater
                        {
                            model: Ossia.Instances {
                                node: "leText";
                                count: 7;
                                parentNode: rectnode;
                                onCountChanged: console.log("txt count: " + count)
                            }

                            delegate: Text{
                                font.pointSize: 20
                                text: "Index: " + index

                                Ossia.Node { node: "leText." + index; id: textnode }
                                Ossia.Property on text { }

                            }
                        }
                    }


                }
            }
        }
    }

    Text {
        id: display
    }

    Component.onCompleted:
    {
        Ossia.SingleDevice.rescan(root)
        Ossia.SingleDevice.saveDevice("file:///tmp/device.json")
        Ossia.SingleDevice.savePreset("file:///tmp/preset.json")


        var xhr = new XMLHttpRequest;
        xhr.open("GET", "/tmp/device.json");
        xhr.onreadystatechange = function() {
        if (xhr.readyState == XMLHttpRequest.DONE) {
         display.text = xhr.responseText;
        }
        };
        xhr.send();

        //console.log("loading")
        //Ossia.SingleDevice.setReadPreset(true);
        //Ossia.SingleDevice.loadPreset(root, "file:///tmp/preset.json")
        //console.log("loaded")
    }
}
