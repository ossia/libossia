import QtQuick 2.0
import org.ossia 1.0

QtObject
{
    function openListening(address) {

    }

    function closeListening(address) {

    }

    function createTree() {
        return [ {
                    name: "metabot",
                    children: [
                        {
                            name: "start",
                            type:  Ossia.Impulse,
                            request: name,
                            access: Ossia.Set
                        },
                        {
                            name: "stop",
                            type:  Ossia.Impulse,
                            request: name,
                            access: Ossia.Set
                        },
                        {
                            name: "back",
                            type:  Ossia.Impulse,
                            request: name,
                            access: Ossia.Set
                        },
                        {
                            name: "gait",
                            type:  Ossia.Impulse,
                            request: name,
                            access: Ossia.Set
                        },

                        {
                            name: "dx",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: -300,
                            max: 300,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "dy",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: -300,
                            max: 300,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "h",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: -120,
                            max: 30,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "turn",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: -300,
                            max: 300,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "crab",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "backleg",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "freq",
                            type: Ossia.Float,
                            request: name + " $val",
                            min: 0,
                            max: 3,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },

                        {
                            name: "alt",
                            type: Ossia.Int,
                            request: name + " $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        }
                    ]
                },
                {
                    name: "toto",
                    type: "float"
                }
                ];
    }
}
