import QtQuick 2.7
import Ossia 1.0 as Ossia

Ossia.NodeImpl
{
    Component.onDestruction: reset_parent()
}
