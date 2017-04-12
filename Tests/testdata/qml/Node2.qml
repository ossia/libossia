import QtQuick 2.7
import Ossia 1.0 as Ossia

Ossia.Node
{
    Component.onDestruction: reset_parent()
}
