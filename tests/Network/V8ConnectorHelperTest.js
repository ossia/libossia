// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// Test of the ossia.v8connector.js helper outside Max, with a fake patcher and
// a fake Maxobj. Run with: node tests/Network/V8ConnectorHelperTest.js

"use strict";

const test = require("node:test");
const assert = require("node:assert");
const path = require("node:path");

const OssiaConnector = require(path.join(
    __dirname, "..", "..", "src", "ossia-max", "javascript", "ossia.v8connector.js"));

// A Maxobj that records the messages it receives.
function fakeObject() {
  return {
    valid: 1,
    received: [],
    message: function () { this.received.push(Array.prototype.slice.call(arguments)); },
  };
}

// The globals of a v8 or v8ui script: the patcher, and error() for the console.
function setup() {
  const objects = { grid: fakeObject() };
  const errors = [];
  global.patcher = {
    parentpatcher: null,
    getnamed: function (name) { return objects[name] || null; },
  };
  global.error = function (text) { errors.push(text); };
  return { objects, errors };
}

// The grid of the recipe: a click creates cell/x/y, a second click removes it.
function makeGrid() {
  const grid = new OssiaConnector("grid");
  const cells = new Set();
  grid.toggle = function (x, y) {
    const address = "cell/" + x + "/" + y;
    if (cells.has(address)) {
      cells.delete(address);
      grid.remove(address);
    } else {
      cells.add(address);
      grid.add(address, { type: "float", min: 0, max: 1, default: 0 });
    }
  };
  return grid;
}

test("a first click sends add with the attributes", () => {
  const { objects } = setup();
  const grid = makeGrid();
  grid.toggle(3, 4);
  assert.deepStrictEqual(objects.grid.received, [
    ["add", "cell/3/4", "@type", "float", "@min", 0, "@max", 1, "@default", 0],
  ]);
});

test("a second click sends remove", () => {
  const { objects } = setup();
  const grid = makeGrid();
  grid.toggle(3, 4);
  grid.toggle(3, 4);
  assert.deepStrictEqual(objects.grid.received[1], ["remove", "cell/3/4"]);
  assert.strictEqual(objects.grid.received.length, 2);
});

test("a removed address is not replayed on ready", () => {
  const { objects } = setup();
  const grid = makeGrid();
  grid.toggle(3, 4);
  grid.toggle(1, 2);
  grid.toggle(3, 4);
  objects.grid.received = [];
  // the object was deleted and recreated: the new one announces itself
  objects.grid = fakeObject();
  assert.strictEqual(grid.dispatch("ready", []), true);
  assert.deepStrictEqual(objects.grid.received, [
    ["add", "cell/1/2", "@type", "float", "@min", 0, "@max", 1, "@default", 0],
  ]);
});

test("refused calls onRefused and reports in the console", () => {
  const { errors } = setup();
  const grid = makeGrid();
  const refusals = [];
  grid.onRefused = function (address, reason) { refusals.push([address, reason]); };
  assert.strictEqual(
      grid.dispatch("refused", ["cell/3/4", "unknown", "address", "'cell/3/4'"]), true);
  assert.deepStrictEqual(refusals, [["cell/3/4", "unknown address 'cell/3/4'"]]);
  assert.strictEqual(errors.length, 1);
  assert.match(errors[0], /refused cell\/3\/4: unknown address 'cell\/3\/4'/);
});
