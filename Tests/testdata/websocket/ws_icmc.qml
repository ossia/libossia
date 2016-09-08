import QtQuick 2.0
import org.ossia 1.0

QtObject
{
    function openListening(address) {

    }

    function closeListening(address) {

    }

    function onMessage(message)
    {
        console.log(message);
        try {
        var res = JSON.parse(message);
        } catch(e) { }

        console.log(res.value);
        var arr = new Array;

        if(res.hasOwnProperty("speed"))
        {
            arr.push({ address: "/speed", value: res.speed });
        }
        if(res.hasOwnProperty("next"))
        {
            arr.push({ address: "/next", value: res.next });
        }

        return arr;
    }

    function createTree() {
        return [ {
                    name: "speed",
                    type: Ossia.Float,
                    min: 0.01,
                    max: 2
                },
                {
                    name: "next",
                    type: Ossia.Int,
                    min: 0,
                    max: 5
                } ];
    }
}
