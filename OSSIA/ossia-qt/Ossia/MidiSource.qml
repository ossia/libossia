import QtQuick 2.0
import Ossia 1.0 as Ossia

Ossia.Device
{
    property int midiPort: 0
    onMidiPortChanged: openMIDIInputDevice(midiPort)
}
