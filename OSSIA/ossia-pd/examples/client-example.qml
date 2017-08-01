import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import CreativeControls 1.0
import Ossia 1.0 as Ossia

/**
 * This file presents an example of usage of the controls
 * provided by the qml-creative-controls library, with libossia and PureData.
 *
 * In the current example, the application we are writing acts as a client:
 * that is, it connects to existing adresses / parameters of a remote
 * software / hardware. It is meant to be used with device-example.pd patch.
 * Open the device-example.pd patch in PureData
 * then open this QML application with qmlscene for example.
 */

Rectangle {
    width: 800
    height: 500

    id: root

    Row {
        id: messageRow
        y: 10
        spacing: 10
        Text
        {
            id: connectionInfo
            font.pointSize: 12
            text: "Open Puredata patch 'device-example.pd' and restart this application"
            color: "#FF0000"
        }

        Button{
            id: connectBtn
            height: 20
            text: "Connect"
            onClicked: connect()
        }
    }


    Ossia.OSCQueryClient
    {
        id: oscqDevice
        // Connect to an OSCQuery server. See for instance OssiaServerExample.qml
        // or libossia's oscquery_publication_example
        address: "ws://127.0.0.1:5678"

        // TODO how to get notified when connection is lost ?
    }

    Column
    {
        y: messageRow.y + 30

        GridLayout
        {
            rowSpacing: 5
            columnSpacing: 10

            Text {
                Layout.column: 0
                Layout.row: 0
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: "modratio"
            }
            AngleSlider {
                id: modRatioSlider
                Layout.column: 0
                Layout.row: 1
                Layout.alignment: Layout.Center
                Ossia.Binding {
                    // Scale the angle
                    on:  10 * (180 + parent.angle) / 360

                    // Send the value to '/modratio' node
                    node: '/modratio'
                    device: oscqDevice

                }
            }
            Text {
                Layout.column: 0
                Layout.row: 2
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: modRatioSlider.angle.toFixed(2)
                Ossia.Callback
                {
                    device: oscqDevice
                    onValueChanged: parent.text = value.toFixed(2)
                    node: '/modratio'
                }
            }

            Text {
                Layout.column: 1
                Layout.row: 0
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: "modgain"
            }
            AngleSlider {
                id: modGainSlider
                Layout.column: 1
                Layout.row: 1
                Layout.alignment: Layout.Center
                Ossia.Binding {
                    // Scale the angle
                    on:  24 * (180 + parent.angle) / 360 - 12

                    // Send the value to '/modgain' node
                    node: '/modgain'
                    device: oscqDevice

                }
            }
            Text {
                Layout.column: 1
                Layout.row: 2
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: modGainSlider.angle.toFixed(2)
                Ossia.Callback
                {
                    device: oscqDevice
                    onValueChanged: parent.text = value.toFixed(2)
                    node: '/modgain'
                }
            }

            Text {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: "attack"
            }
            AngleSlider {
                id: attackSlider
                Layout.column: 2
                Layout.row: 1
                Layout.alignment: Layout.Center
                Ossia.Binding {
                    // Scale the angle
                    on:  500 * (180 + parent.angle) / 360

                    // Send to '/attack' node
                    node: '/attack'
                    device: oscqDevice

                }
            }
            Text {
                Layout.column: 2
                Layout.row: 2
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: attackSlider.angle.toFixed(2)
                Ossia.Callback
                {
                    device: oscqDevice
                    onValueChanged: parent.text = value.toFixed(2)
                    node: '/attack'
                }
            }

            Text {
                Layout.column: 3
                Layout.row: 0
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: "decay"
            }
            AngleSlider {
                id: decaySlider
                Layout.column: 3
                Layout.row: 1
                Layout.alignment: Layout.Center
                Ossia.Binding {
                    // Scale the angle
                    on:  1000 * (180 + parent.angle) / 360

                    // Send to /decay node
                    node: '/decay'
                    device: oscqDevice

                }
            }
            Text {
                Layout.column: 3
                Layout.row: 2
                Layout.alignment: Layout.Center
                font.pointSize: 12
                text: decaySlider.angle.toFixed(2)
                Ossia.Callback
                {
                    device: oscqDevice
                    onValueChanged: parent.text = value.toFixed(2)
                    node: '/decay'
                }
            }
        }

        Keyboard
        {
            id: kbd
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Layout.Center

            width: 400
            height: 200

            firstKey: 24
            lastKey: 53

            Ossia.Binding {
                // Scale the angle between 0 - 127
                on:  Qt.point(kbd.pressedKeys[0].key.toFixed(0), kbd.pressedKeys[0].vel.toFixed(0))

                // The value changes are sent to CC 34 on channel 1
                node: '/note'
                device: oscqDevice
            }
        }
        Text {
            Layout.column: 1
            Layout.row: 0
            Layout.alignment: Qt.AlignLeft
            font.pointSize: 20
            text:
            {
                if(kbd.pressedKeys.length > 0 && kbd.pressedKeys[0].key !== undefined && kbd.pressedKeys[0].vel !== undefined)
                    "Keyboard: " + kbd.pressedKeys[0].key.toFixed(0) + ", " + kbd.pressedKeys[0].vel.toFixed(0)
                else
                    "You can use your computer keyboard to play"
            }
        }
    }

    // map keycode to pitch
    function key2pitch(key){
        var pitch
        switch(key) {
            case 81:
                pitch = 0
                break;
            case 90:
                pitch = 1
                break;
            case 83:
                pitch = 2
                break;
            case 69:
                pitch = 3
                break;
            case 68:
                pitch = 4
                break;
            case 70:
                pitch = 5
                break;
            case 84:
                pitch = 6
                break;
            case 71:
                pitch = 7
                break;
            case 89:
                pitch = 8
                break;
            case 72: // 'h'
                pitch = 9
                break;
            case 85: // 'u'
                pitch = 10
                break;
            case 74: // 'j'
                pitch = 11
                break;
            case 75: // 'k'
                pitch = 12
                break;
        }
        return pitch
    }

    focus: true
    Keys.onPressed: {
        var pitch = key2pitch(event.key)
        if (pitch !== undefined){
            var note = { key: pitch + kbd.firstKey, vel: 100 }
            //kbd.pressedKeys = [ note ]
            kbd.setPressed(note)
            console.log(note.key + " " + note.vel)
        }
    }

    Keys.onReleased: {
        var pitch = key2pitch(event.key)
        if (pitch !== undefined && kbd.isPressed(pitch + kbd.firstKey)){
            kbd.setPressed([])
        }
    }

    Component.onCompleted:
    {
        // Call this at the end to set-up the connections
        connect()
    }

    function connect() {
        if (oscqDevice.openOSCQueryClient(oscqDevice.address, oscqDevice.localPort)){
            connectionInfo.text = "connected to device " + oscqDevice.address
            connectionInfo.color = "#008800"
            connectBtn.visible = false
            console.log("connected to " + oscqDevice.address)
        } else {
            connectionInfo.text = "open a device and reconnect"
            connectionInfo.color = "#FF0000"
        }

        oscqDevice.remap(root)
    }

    Dialog {
        visible: false
        title: "Blue sky dialog"

        contentItem: Rectangle {
            color: "lightskyblue"
            implicitWidth: 400
            implicitHeight: 100
            Text {
                text: "Hello blue sky!"
                color: "navy"
                anchors.centerIn: parent
            }
        }
    }
}
