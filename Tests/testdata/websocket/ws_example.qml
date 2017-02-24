import QtQuick 2.0
import Ossia 1.0

QtObject
{

    function onMessage(message) {
        console.log(message);
        var res = JSON.parse(message);
        console.log(res.value);
        if(res.name === "toto")
        {
            return [ { address: "/toto", value: res.value } ];
        }
        return { };
    }

    function createTree() {
        return [ {
                    name: "tata",
                    children: [
                        {
                            name: "tutu",
                            request: "{ \"name\": \"toto\", \"value\": $val }",
                            type: Ossia.Float,
                            unit: "color.rgb"
                        },
                        {
                            name: "titi",
                            request: function (value) {
                                return JSON.stringify({ tutu: value })
                            },

                            openListening: function () {
                                return "open titi listening";
                            },

                            closeListening: function () {
                                return "close titi listening";
                            }
                        }
                    ]
                },
                {
                    name: "toto",
                    type: Ossia.Float
                }
                ];
    }
}
