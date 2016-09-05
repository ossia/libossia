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
                    name: "tata",
                    children: [
                        {
                            name: "tutu",
                            request: JSON.stringify({ tutu: 123 }),
                            answer: function (json) {
                                console.log(json)
                                return [ { address: "/toto", value: 2.3 } ]
                            }
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
