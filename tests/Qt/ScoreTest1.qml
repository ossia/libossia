import QtQuick 2.0
import QtQuick.Controls 2.2
import Ossia 1.0 as Ossia

Item
{
    Ossia.OSCQueryClient {
        id: client
        name: "foo"
    }

    Rectangle {
        id: rec
        color: "red"
        height: 200
        Ossia.Property on height { id: rec_height; device: Ossia.SingleDevice }
        width: 200
        Ossia.Property on width { id: rec_width; device: Ossia.SingleDevice }
        x: 100
        y: 100
        Ossia.Property on x { device: Ossia.SingleDevice }
        Ossia.Property on y { device: Ossia.SingleDevice }
        Ossia.Property on rotation { id: rec_rot; device: Ossia.SingleDevice }
    }

    Text {
        x: 300
        y: 30
        text: "banana"
        Ossia.Property on scale { node: "/text/scale"; device: Ossia.SingleDevice }
        Ossia.Property on rotation { node: "/text/rotation"; device: Ossia.SingleDevice }
        Ossia.Property on font.letterSpacing { node: "/text/spacing"; device: Ossia.SingleDevice; }
    }

    Slider {
        id: sld
    }

    Ossia.Interval {
        id: root
        nominalDuration: 2000
        minDuration: 2000
        maxDuration: Ossia.Duration.Infinite

        Ossia.Automation {
            target: rec_height
            yMin: 10
            yMax: 400
        }
        Ossia.Automation {
            target: rec_width
            yMin: 50
            yMax: 100
        }

        Ossia.Scenario {
            id: scenario
            startSync: Ossia.Sync { id: s1 }

            Ossia.Interval {
                follows: s1
                precedes: c2
                nominalDuration: 1000
                minDuration: 1000
                maxDuration: Ossia.Duration.Infinite

                Ossia.Automation {
                    target: rec_rot
                    yMin: 100
                    yMax: 500
                }
            }

            Ossia.Sync {
                expr: sld.position > 0.5
                Ossia.Condition {
                    id: c2
                }
            }

            Ossia.Interval {
                follows: c2
                precedes: s3
                nominalDuration: 5000
                minDuration: 5000
                maxDuration: 5000

                Ossia.Script {
                    script: [ Ossia.Make.message(Ossia.SingleDevice, "/x", rec.x + 10 * Math.random())
                            , Ossia.Make.message(Ossia.SingleDevice, "/y", rec.y + 10 * Math.random()) ];
                }

                Ossia.Automation {
                    target: Ossia.Writer { node: "/text/scale"; device: client }
                    yMin: 10.0
                    yMax: 100.0

                    Ossia.Breakpoint { x: 0.; y: 10; type: Easing.InOutBounce }
                    Ossia.Breakpoint { x: 0.2; y: 50;}
                    Ossia.Breakpoint { x: 0.5; y: 15; type: Easing.InQuint }
                    Ossia.Breakpoint { x: 0.7; y: 5 }
                    Ossia.Breakpoint { x: 1.; y: 15;  type: Easing.InOutElastic }
                }
                Ossia.Automation {
                    target: "/text/spacing"
                    yMin: 0
                    yMax: 10
                }
                Ossia.Automation {
                    target: "/text/rotation"
                    yMin: 0
                    yMax: 100
                }
            }

            Ossia.Sync { id: s3 }

            Ossia.Interval {
                follows: s3
                precedes: s4

                nominalDuration: 10000
                minDuration: 10000
                maxDuration: 10000

                Ossia.Loop {
                    interval: Ossia.Interval {
                        id: loopItv
                        nominalDuration: 1000
                        minDuration: 1000
                        maxDuration: 1000

                        Ossia.Automation {
                            target: "/text/rotation"
                            Ossia.Breakpoint { x: 0.; y: 1. }
                            Ossia.Breakpoint { x: 0.5; y: 100.; type: Easing.InQuart }
                            Ossia.Breakpoint { x: 1.; y: 1.; type: Easing.OutQuart }
                        }
                    }
                }
            }
            Ossia.Sync { id: s4 }
        }
    }

    Text { x: 30; y: 300; text: loopItv.playDuration }
    id: rt
    Component.onCompleted: {
        Ossia.SingleDevice.recreate(rt);
        Ossia.SingleDevice.openOSCQueryServer(5678, 1234);
        client.openOSCQueryClient("ws://127.0.0.1:5678", 7890)
        client.remap(rt);

        root.play()
    }
}
