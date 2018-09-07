var ossia = require('bindings')('ossia-node');


var proto = new ossia.OscQueryServer(1234, 5678);
var device = new ossia.Device(proto, "ossia-node");

var node = device.getRootNode();

console.log("my name is " + node.getName());