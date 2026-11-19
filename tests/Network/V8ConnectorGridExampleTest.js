// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// Test of the grid example script (examples/ossia.v8connector.grid.js) outside
// Max, with a fake v8ui (box, mgraphics), a fake patcher and a fake connector.
// Run with: node tests/Network/V8ConnectorGridExampleTest.js

"use strict";

const test = require("node:test");
const assert = require("node:assert");
const fs = require("node:fs");
const path = require("node:path");
const vm = require("node:vm");

const ossiaMax = path.join(__dirname, "..", "..", "src", "ossia-max");
const helperPath = path.join(ossiaMax, "javascript", "ossia.v8connector.js");
const scriptPath = path.join(ossiaMax, "examples", "ossia.v8connector.grid.js");

// The v8ui running the grid script: a 240 x 240 box, its arguments, the patcher
// holding the connector named "grid", and the outlet that comes back into it.
function load() {
  const connector = {
    valid: 1,
    received: [],
    message: function () { this.received.push(Array.prototype.slice.call(arguments)); },
  };
  const errors = [];
  let redraws = 0;
  const mgraphics = new Proxy({}, {
    get: (target, name) => {
      if (name === "redraw") return () => { redraws++; };
      if (name in target) return target[name];
      return () => {};
    },
    set: (target, name, value) => { target[name] = value; return true; },
  });
  const context = {
    jsarguments: ["ossia.v8connector.grid.js", "grid", 8],
    box: { rect: [0, 0, 240, 240] },
    mgraphics,
    patcher: { parentpatcher: null, getnamed: (name) => (name === "grid" ? connector : null) },
    error: (text) => errors.push(text),
    post: () => {},
    messagename: "",
    arrayfromargs: (args) => Array.prototype.slice.call(args),
  };
  context.require = (name) => {
    assert.strictEqual(name, "ossia.v8connector");
    const module = { exports: {} };
    vm.runInContext(
        "(function (module) {" + fs.readFileSync(helperPath, "utf8") + "\n})", context)(module);
    return module.exports;
  };
  vm.createContext(context);
  vm.runInContext(fs.readFileSync(scriptPath, "utf8"), context, { filename: scriptPath });
  // The connector's left outlet into the v8ui: Max calls anything() with
  // messagename set.
  const outlet = (address, ...values) => {
    context.messagename = address;
    context.anything(...values);
  };
  // A click at the centre of a cell (30 x 30 pixels).
  const click = (x, y) => context.onclick(x * 30 + 15, y * 30 + 15);
  return { context, connector, errors, outlet, click, redraws: () => redraws };
}

const ADD_3_4 = ["add", "cell/3/4", "@type", "float", "@min", 0, "@max", 1, "@default", 0];

test("a click on an empty cell declares cell/x/y", () => {
  const { connector, click } = load();
  click(3, 4);
  assert.deepStrictEqual(connector.received, [ADD_3_4]);
});

test("a second click on the cell removes it", () => {
  const { connector, click } = load();
  click(3, 4);
  click(3, 4);
  assert.deepStrictEqual(connector.received, [ADD_3_4, ["remove", "cell/3/4"]]);
});

test("a click outside the grid does nothing", () => {
  const { connector, context } = load();
  context.onclick(-5, 10);
  context.onclick(250, 10);
  assert.deepStrictEqual(connector.received, []);
});

test("on ready, the cells still on come back, not the removed ones", () => {
  const { connector, click, outlet } = load();
  click(3, 4);
  click(1, 2);
  click(3, 4);
  connector.received = [];
  outlet("ready");
  assert.deepStrictEqual(connector.received, [
    ["add", "cell/1/2", "@type", "float", "@min", 0, "@max", 1, "@default", 0],
  ]);
});

test("a value changed elsewhere redraws the cell; a removed cell ignores it", () => {
  const { click, outlet, redraws } = load();
  click(3, 4);
  const before = redraws();
  outlet("cell/3/4", 0.75);
  assert.strictEqual(redraws(), before + 1);
  click(3, 4);
  const after = redraws();
  outlet("cell/3/4", 0.5);
  assert.strictEqual(redraws(), after);
});
