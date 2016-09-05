import QtQuick 2.0
import org.ossia 1.0

QtObject
{
    function openListening(address) {

    }

    function closeListening(address) {

    }

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
                            type: Ossia.Float
                        },
                        {
                            name: "titi",
                            request: function (value) {
                                return JSON.stringify({ tutu: value })
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
