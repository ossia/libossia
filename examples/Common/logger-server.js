var WebSocketServer = require('ws').Server
var wss = new WebSocketServer({port: 1337});

wss.on('connection', function(ws) {
    ws.on('message', function(message) {
        console.log('received: %s', message);
    });
});

