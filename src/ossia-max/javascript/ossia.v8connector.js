// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// ossia.v8connector.js: helper for v8 and v8ui scripts around [ossia.v8connector].
//
// The script reaches the object without a patch cord, through its scripting
// name (the "varname" set in the inspector), or through a Maxobj:
//
//   const OssiaConnector = require("ossia.v8connector");
//   const synth = new OssiaConnector("synth");           // scripting name, or a Maxobj
//   synth.declare({
//     gain:   { type: "float", min: -60, max: 6, unit: "gain.db", default: -12 },
//     cutoff: { type: "float", range: [20, 20000], unit: "time.hz" },
//     voices: { type: "int", min: 1, max: 16, default: 4 },
//   });
//   synth.on("gain", (v) => { gain = v; mgraphics.redraw(); });
//   synth.set("gain", -6);
//   synth.remove("voices");
//
// bind subscribes to a parameter that exists elsewhere, declared by another
// instance or by an ossia.parameter, without creating anything. The address
// follows the rules of ossia.remote: relative to the enclosing model or
// device, absolute with a leading slash, or device:/address.
//
//   const mirror = new OssiaConnector("mirror");
//   mirror.bind("synth/gain");
//   mirror.on("synth/gain", (v) => { gain = v; });
//   mirror.set("synth/gain", -6);                 // pushes to the owner's parameter
//   mirror.unbind("synth/gain");
//
// Values that change by another source (a remote client, another ossia object,
// another script) leave the object's left outlet as "<address> <values...>".
// Patch that outlet into the script's inlet and forward them from anything():
//
//   function anything() { synth.dispatch(messagename, arrayfromargs(arguments)); }
//
// The object sends "ready" on that outlet once it has registered. The helper
// then replays every declaration and bind it knows, so that an object deleted
// and recreated (undo, paste) gets them again. The helper never keeps a stale
// reference: it looks the object up by name at each send, and checks that a
// Maxobj it was given is still valid.
//
// A value the parameter changed (clip to the declared range, type conversion)
// comes back on the left outlet, so that the script shows the effective value.
//
// A numeric type (float, int, vec2f, vec3f, vec4f) needs min and max, or
// range: the object refuses the declaration otherwise. A unit imposes its type
// (color.rgb is vec3f, color.rgba vec4f): a declaration whose type differs is
// refused too. clip accepts "none", the OSCQuery name of the free mode.
//
// Every refusal (add, bind, set, remove, unbind) also comes out of the left
// outlet as "refused <address> <reason>". The helper reports it in the Max
// console and calls connector.onRefused(address, reason) when it is set:
//
//   synth.onRefused = (address, reason) => { post("refused", address, reason, "\n"); };
//
// "ready" and "refused" are therefore reserved addresses.

function OssiaConnector(target) {
  this.handlers = {};
  this.declarations = {}; // address -> attributes, replayed on "ready"
  this.declarationOrder = [];
  this.bindings = []; // addresses, replayed on "ready"
  this.onRefused = null; // optional function (address, reason)
  this.target = null;
  this.name = null;
  if (typeof target === "string") {
    this.name = target;
  } else if (target) {
    this.target = target;
    if (target.varname) this.name = String(target.varname);
  }
}

function isValid(obj) {
  if (!obj) return false;
  if (typeof obj.valid !== "undefined") return !!obj.valid;
  return true;
}

// Resolve the object every time: the one found earlier may have been deleted
// (undo, paste) and replaced by another one with the same scripting name.
OssiaConnector.prototype.object = function () {
  if (this.target && isValid(this.target) && !this.name) return this.target;
  if (!this.name) return null;
  var p = this.patcher();
  while (p) {
    var obj = p.getnamed(this.name);
    if (obj && isValid(obj)) return obj;
    p = p.parentpatcher;
  }
  return null;
};

OssiaConnector.prototype.patcher = function () {
  if (typeof patcher !== "undefined" && patcher) return patcher;
  if (typeof this_patcher !== "undefined") return this_patcher;
  return null;
};

OssiaConnector.prototype.send = function (args) {
  var obj = this.object();
  if (!obj) {
    error("ossia.v8connector: no object named '" + this.name + "' in this patcher or above\n");
    return false;
  }
  obj.message.apply(obj, args);
  return true;
};

function pushValue(args, value) {
  if (Array.isArray(value)) {
    for (var k = 0; k < value.length; k++) args.push(value[k]);
  } else if (typeof value === "boolean") {
    args.push(value ? 1 : 0);
  } else {
    args.push(value);
  }
}

function addMessage(address, attrs) {
  var args = ["add", address];
  var names = Object.keys(attrs || {});
  for (var j = 0; j < names.length; j++) {
    var value = attrs[names[j]];
    if (value === undefined || value === null) continue;
    args.push("@" + names[j]);
    pushValue(args, value);
  }
  return args;
}

// declare({ address: { type, min, max, range, unit, default, clip, mode,
//                      description, tags, priority, rate, critical, invisible,
//                      mute, repetitions, enable, recall_safe } })
// Each entry becomes one "add" message with the same attribute names as
// ossia.parameter. The declarations are kept and replayed on "ready".
OssiaConnector.prototype.declare = function (spec) {
  var addresses = Object.keys(spec);
  for (var i = 0; i < addresses.length; i++) {
    this.add(addresses[i], spec[addresses[i]] || {});
  }
};

OssiaConnector.prototype.add = function (address, attrs) {
  if (!(address in this.declarations)) this.declarationOrder.push(address);
  this.declarations[address] = attrs || {};
  this.send(addMessage(address, this.declarations[address]));
};

OssiaConnector.prototype.set = function (address) {
  var args = ["set", address];
  for (var i = 1; i < arguments.length; i++) pushValue(args, arguments[i]);
  this.send(args);
};

OssiaConnector.prototype.remove = function (address) {
  delete this.declarations[address];
  this.declarationOrder = this.declarationOrder.filter(function (a) { return a !== address; });
  this.send(["remove", address]);
};

OssiaConnector.prototype.clear = function () {
  this.declarations = {};
  this.declarationOrder = [];
  this.send(["clear"]);
};

// bind(address): follow a parameter that exists elsewhere; set and on work on it.
OssiaConnector.prototype.bind = function (address) {
  if (this.bindings.indexOf(address) < 0) this.bindings.push(address);
  this.send(["bind", address]);
};

OssiaConnector.prototype.unbind = function (address) {
  this.bindings = this.bindings.filter(function (a) { return a !== address; });
  this.send(["unbind", address]);
};

// Send again every declaration and bind this helper knows. Called on "ready";
// the object accepts a declaration it already has, unchanged.
OssiaConnector.prototype.replay = function () {
  for (var i = 0; i < this.declarationOrder.length; i++) {
    var address = this.declarationOrder[i];
    this.send(addMessage(address, this.declarations[address]));
  }
  for (var j = 0; j < this.bindings.length; j++) {
    this.send(["bind", this.bindings[j]]);
  }
};

// on(address, handler): handler(values...) runs when the value changes by another source.
OssiaConnector.prototype.on = function (address, handler) {
  this.handlers[address] = handler;
};

OssiaConnector.prototype.off = function (address) {
  delete this.handlers[address];
};

// dispatch(address, values): call from anything() with the messages of the left outlet.
OssiaConnector.prototype.dispatch = function (address, values) {
  if (address === "ready") {
    this.replay();
    return true;
  }
  if (address === "refused") {
    var refusedAddress = values.length > 0 ? String(values[0]) : "";
    var reason = values.length > 1 ? values.slice(1).join(" ") : "";
    error("ossia.v8connector: refused " + refusedAddress + ": " + reason + "\n");
    if (typeof this.onRefused === "function") this.onRefused(refusedAddress, reason);
    return true;
  }
  var handler = this.handlers[address];
  if (!handler) return false;
  handler.apply(null, values);
  return true;
};

if (typeof module !== "undefined") module.exports = OssiaConnector;
