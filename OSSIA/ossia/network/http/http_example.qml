import QtQuick 2.0

QtObject
{
    function OpenListening(address) {

    }

    function CloseListening(address) {

    }

    function CreateTree() {
        return {
            name: "tata",
            children: [
                {
                    name: "tutu",
                    request: "https://jsonplaceholder.typicode.com/todos",
                    answer: function (json) { }
                },
                {
                    name: "titi",
                    request: "https://jsonplaceholder.typicode.com/todos/$val",
                    answer: function (json) { }
                }
            ]
        };
    }
}
