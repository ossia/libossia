import Ossia 1.0 as Ossia
import QtQuick 2.5
import QtQml 2.11

Item {
    Repeater {
        model: Ossia.Instances {
            id: rp;
            node: "foo";
            onCountChanged: console.log("Root model count: " + count)
        }
        Item {
            id: tutu
            objectName: "tutu" + index
            Ossia.Node { node: "foo." + index; id: n }
            Ossia.Property on x { parentNode: n }
            Ossia.Property on y { parentNode: n }
            Rectangle {
                id: tata
                parent: tutu
                objectName: "root" + index;
                onObjectNameChanged: console.log("root name: " + objectName)

                Ossia.Node {
                    id: sub
                    node: "tata"
                    parentNode: n
                }
                Repeater {
                    model: Ossia.Instances {
                          id: rp2;
                          node: "bar";
                          onCountChanged: console.log("RectChild model count: " + count)
                          parentNode: sub;
                          }
                    Rectangle {
                        id: prnt
                        parent: tata
                        objectName: tata.objectName + "RectChild" + index;
                        onObjectNameChanged: console.log("RectChild name: " + objectName)
                        Ossia.Node { node: "bar." + index; id: subn; parentNode: sub }
                        Ossia.Property on x { parentNode: subn }
                        Ossia.Property on y { parentNode: subn }
                        Component.onCompleted: console.log(" RECT CHILD " + objectName + "CREATED")
                        Component.onDestruction: console.log(" RECT CHILD " + objectName + "DESTROYED")

                        Rectangle {
                            id: rccRec
                            objectName: "RectChildChild" + index
                            Ossia.Node {
                                id: subsub
                                node: "papa"
                                parentNode: subn
                            }
                            Instantiator {
                                model: Ossia.Instances { node: "baz"; parentNode: subsub;  }
                                Item {
                                    objectName: rccRec.objectName + "-child" + index;
                                    parent: rccRec
                                    //onObjectNameChanged: console.log("Child name: " + objectName)
                                    Ossia.Node { node: "baz." + index; id: subsubn; parentNode: subsub }
                                    Ossia.Property on x { parentNode: subsubn }
                                    Ossia.Property on y { parentNode: subsubn }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
