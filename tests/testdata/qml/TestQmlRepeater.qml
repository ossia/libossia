import QtQuick 2.10

Item {
    Instantiator {
        model: 3
        Item {
            objectName: "Root"
            Rectangle {
                objectName: "Rect"

                Instantiator {
                    model: 5
                    Rectangle {
                        objectName: "Child"
                    }
                }
            }
        }
    }
}
