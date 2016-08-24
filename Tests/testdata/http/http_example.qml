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
                            request: "https://jsonplaceholder.typicode.com/todos",
                            answer: function (json) {
                                console.log(json)
                                return [ { address: "/toto", value: 2.3 } ]
                            }
                        },
                        {
                            name: "titi",
                            request: "https://jsonplaceholder.typicode.com/todos/$val"
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
