import Ossia 1.0 as Ossia

Ossia.Device
{
    property int oscPort: 1234
    property int wsPort: 5678

    onOscPortChanged: openOSCQueryServer()
    onWsPortChanged: openOSCQueryServer()
}
