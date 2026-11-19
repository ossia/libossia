// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// Test of the load example scripts (examples/ossia.v8connector.load.*.js)
// outside Max, with a fake v8 / v8ui, a fake patcher and fake connectors.
// Run with: node tests/Network/V8ConnectorLoadExampleTest.js

"use strict";

const test = require("node:test");
const assert = require("node:assert");
const fs = require("node:fs");
const path = require("node:path");
const vm = require("node:vm");

const ossiaMax = path.join(__dirname, "..", "..", "src", "ossia-max");
const helperPath = path.join(ossiaMax, "javascript", "ossia.v8connector.js");

function load(script, args, connectorName) {
  const connector = {
    valid: 1,
    received: [],
    message: function () { this.received.push(Array.prototype.slice.call(arguments)); },
  };
  const tasks = [];
  const context = {
    jsarguments: [script].concat(args),
    box: { rect: [0, 0, 700, 260] },
    mgraphics: new Proxy({}, { get: (t, n) => (n in t ? t[n] : () => {}), set: (t, n, v) => { t[n] = v; return true; } }),
    patcher: { parentpatcher: null, getnamed: (name) => (name === connectorName ? connector : null) },
    error: () => {},
    post: () => {},
    messagename: "",
    arrayfromargs: (a) => Array.prototype.slice.call(a),
    sent: [],
    outlet: function () { context.sent.push(Array.prototype.slice.call(arguments)); },
    Task: function (fn) { this.fn = fn; this.interval = 0; this.repeat = () => {}; this.schedule = () => {}; this.cancel = () => {}; tasks.push(this); },
  };
  context.require = () => {
    const module = { exports: {} };
    vm.runInContext("(function (module) {" + fs.readFileSync(helperPath, "utf8") + "\n})", context)(module);
    return module.exports;
  };
  vm.createContext(context);
  const file = path.join(ossiaMax, "examples", script);
  vm.runInContext(fs.readFileSync(file, "utf8"), context, { filename: file });
  const outlet = (address, ...values) => { context.messagename = address; context.anything(...values); };
  return { context, connector, tasks, outlet };
}

test("the emitter declares 3000 leaves, then sets them all at each bang", () => {
  const { context, connector } = load("ossia.v8connector.load.emitter.js", ["load_emitter", 3000], "load_emitter");
  context.loadbang();
  const adds = connector.received.filter((m) => m[0] === "add");
  assert.strictEqual(adds.length, 3000);
  assert.deepStrictEqual(adds[2999], ["add", "p2999", "@type", "float", "@min", 0, "@max", 1, "@default", 0.5]);
  connector.received = [];
  context.bang();
  const sets = connector.received.filter((m) => m[0] === "set");
  assert.strictEqual(sets.length, 3000);
  assert.ok(sets.every((m) => m[2] >= 0 && m[2] <= 1));
  connector.received = [];
  context.bang();
  assert.notDeepStrictEqual(connector.received[0], sets[0]); // the values move
});

test("the emitter announces declared once its leaves exist, in either order", () => {
  // declared first, then the connector registers
  let e = load("ossia.v8connector.load.emitter.js", ["load_emitter", 3000], "load_emitter");
  e.context.loadbang();
  assert.deepStrictEqual(e.context.sent, []);
  e.outlet("ready");
  assert.deepStrictEqual(e.context.sent, [[0, "declared"]]);
  // the connector registered first
  e = load("ossia.v8connector.load.emitter.js", ["load_emitter", 3000], "load_emitter");
  e.outlet("ready");
  assert.deepStrictEqual(e.context.sent, []);
  e.context.loadbang();
  assert.deepStrictEqual(e.context.sent, [[0, "declared"]]);
});

test("the emitter replays its leaves on ready", () => {
  const { context, connector, outlet } = load("ossia.v8connector.load.emitter.js", ["load_emitter", 3000], "load_emitter");
  context.loadbang();
  connector.received = [];
  outlet("ready");
  assert.strictEqual(connector.received.filter((m) => m[0] === "add").length, 3000);
});

test("the receiver binds 3000 addresses and counts the values received", () => {
  const { context, connector, tasks, outlet } = load("ossia.v8connector.load.receiver.js", ["load_receiver", 3000, "load"], "load_receiver");
  context.loadbang();
  const binds = connector.received.filter((m) => m[0] === "bind");
  assert.strictEqual(binds.length, 3000);
  assert.deepStrictEqual(binds[0], ["bind", "load/p0"]);
  for (let i = 0; i < 3000; i++) outlet("load/p" + i, 0.5);
  outlet("load/p9999", 0.5); // not followed: ignored
  // the task of the second publishes the counters
  const second = tasks.find((t) => t.interval === 1000);
  second.fn();
  const frame = tasks.find((t) => t.interval === 33);
  assert.ok(frame);
  let shown = "";
  context.mgraphics.show_text = (s) => { shown = s; };
  context.paint();
  assert.match(shown, /^3000 values\/s, 1 groups\/s/);
});
