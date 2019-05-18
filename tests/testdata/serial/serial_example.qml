/* The folowing code creates a
tree of parameters to control the Metabot v1. The bluetooth device needs to be connected to a serial port. It will then become selectable in the "Port" menu above upon starting score.
On Linux, permission to acces this port requires the user to be added to the dialout group
$ sudo usermod -a -G dialout "your-username"
modemanager can also cause permission errors and might need to be uninstalled
$ sudo apt remove modemmanager
*/

import QtQuick 2.0
import Ossia 1.0 as Ossia

QtObject
{
    function openListening(address) {}

    function closeListening(address) {}

    function createTree() {
        return [ {
                    name: "metabot",
                    children: [
                        {
                            name: "start",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
                        {
                            name: "stop",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
                        {
                            name: "toggleBackLegs",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
            {
                            name: "toggleColorfront",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
                        {
                            name: "toggleCrab",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
                        {
                            name: "trot",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
                        {
                            name: "music",
                            type:  Ossia.Type.Impulse,
                            access: Ossia.Access.Set
                        },
                        {
                            name: "dx",
                            type:  Ossia.Type.Float,
                            request: "dx $val",
                            min: -300,
                            max: 300,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "dy",
                            type:  Ossia.Type.Float,
                            request: "dy $val",
                            min: -300,
                            max: 300,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "h",
                            type:  Ossia.Type.Float,
                            request: "h $val",
                            min: -120,
                            max: 30,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "turn",
                            type:  Ossia.Type.Float,
                            request: "turn $val",
                            min: -300,
                            max: 300,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "crab",
                            type:  Ossia.Type.Float,
                            request: "crab $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "backLeg",
                            type:  Ossia.Type.Float,
                            request: "backLeg $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "freq",
                            type:  Ossia.Type.Float,
                            request: "freq $val",
                            min: 0,
                            max: 3,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "freqLegs",
                            type:  Ossia.Type.Int,
                            request: "freqLegs $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "alt",
                            type:  Ossia.Type.Int,
                            request: "alt $val",
                            min: 0,
                            max: 10,
                            access: Ossia.Access.Set,
                            bounding: Ossia.Bounding.Clip,
                            repetition_filter: Ossia.Repetitions.Filtered
                        },
                        {
                            name: "color",
                            type:  Ossia.Type.Int,
                            request: "color $val",
                            min: 0,
                            max: 8,
                            access: Ossia.Access.Set,
                        }
                    ]
                }
                ];
    }
}
