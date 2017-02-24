import QtQuick 2.0
import Ossia 1.0

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
                            access: Ossia.Set
                        },
                        {
                            name: "stop",
                            type:  Ossia.Impulse,
                            access: Ossia.Set
                        },
                        {
                            name: "back",
                            type:  Ossia.Impulse,
                            access: Ossia.Set
                        },
                        {
                            name: "gait",
                            type:  Ossia.Impulse,
                            access: Ossia.Set
                        },

                        {
                            name: "dx",
                            type: Ossia.Float,
                            request: "dx $0",
                            min: -300,
                            max: 300,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "dy",
                            type: Ossia.Float,
                            request: "dy $0",
                            min: -300,
                            max: 300,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "h",
                            type: Ossia.Float,
                            request: "h $0",
                            min: -120,
                            max: 30,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "turn",
                            type: Ossia.Float,
                            request: "turn $0",
                            min: -300,
                            max: 300,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "crab",
                            type: Ossia.Float,
                            request: "crab $0",
                            min: 0,
                            max: 10,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "backleg",
                            type: Ossia.Float,
                            request: "backleg $0",
                            min: 0,
                            max: 10,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },
                        {
                            name: "freq",
                            type: Ossia.Float,
                            request: "freq $0",
                            min: 0,
                            max: 3,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        },

                        {
                            name: "alt",
                            type: Ossia.Int,
                            request: "alt $0",
                            min: 0,
                            max: 10,
                            access: Ossia.Set,
                            bounding: Ossia.Clip,
                            repetition_filter: Ossia.Off
                        }
                    ]
                }
                ];
    }
}
