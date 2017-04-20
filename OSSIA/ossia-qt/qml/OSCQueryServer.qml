import Ossia 1.0 as Ossia

Ossia.Device
{
    onOscPortChanged: openOSCQueryServer()
    onWsPortChanged: openOSCQueryServer()
}
