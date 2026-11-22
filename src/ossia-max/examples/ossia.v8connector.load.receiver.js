// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// Receiver of the load example of ossia.v8connector: follows the N leaves of
// the emitter through an ossia.client (bind load/p0 to load/p<N-1>) and draws
// them as a cloud of points, leaf i at x = i, y = its value. The values are
// kept in a Float32Array as they arrive; the drawing reads it 30 times per
// second, all the points in one path filled once.
//
// Counters, once per second: values received, and groups of values (one per
// drain of the connector, which outputs what arrived since its last drain).
// The network frames themselves are not visible from Max: the load bench of
// the repository counts them.
//
// Patch: [ossia.v8connector watch] with the scripting name "load_receiver",
// in the patcher of the ossia.client, its left outlet into
// [v8ui ossia.v8connector.load.receiver.js load_receiver 3000 load].
// Arguments, after the script name:
//   1. scripting name of the connector (default "load_receiver")
//   2. number of leaves (default 3000)
//   3. node of the leaves on the server (default "load")

mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;

let OssiaConnector = null;
try {
  OssiaConnector = require("ossia.v8connector");
} catch (e) {
  error("ossia.v8connector.js not found: it lives in the javascript folder of the ossia package\n");
}

const CONNECTOR = jsarguments.length > 1 ? String(jsarguments[1]) : "load_receiver";
const N = jsarguments.length > 2 ? Math.max(1, Math.floor(Number(jsarguments[2]))) : 3000;
const NODE = jsarguments.length > 3 ? String(jsarguments[3]) : "load";
const conn = OssiaConnector ? new OssiaConnector(CONNECTOR) : null;

const values = new Float32Array(N).fill(-1); // -1: nothing received yet
let received = 0;      // values since the last second
let groups = 0;        // groups since the last second
let inGroup = false;
let valuesPerSecond = 0;
let groupsPerSecond = 0;
let bound = 0;         // parameters the connector follows, from its right outlet

function bindAll() {
  if (!conn) return;
  for (let i = 0; i < N; i++) {
    const address = NODE + "/p" + i;
    conn.on(address, (v) => {
      values[i] = v;
      received++;
      if (!inGroup) {
        inGroup = true;
        groups++;
        groupEnd.schedule(0);
      }
    });
    conn.bind(address);
  }
}

// A group ends when the connector's burst of outputs is over.
const groupEnd = new Task(() => { inGroup = false; }, this);
const second = new Task(() => {
  valuesPerSecond = received;
  groupsPerSecond = groups;
  received = 0;
  groups = 0;
}, this);
second.interval = 1000;
const frame = new Task(() => { mgraphics.redraw(); }, this);
frame.interval = 33;

function loadbang() {
  bindAll();
  second.repeat();
  frame.repeat();
}

function notifydeleted() {
  second.cancel();
  frame.cancel();
  groupEnd.cancel();
}

// The connector's outlets: the values (and "ready", "refused") on the left,
// "bind <address> <n>" on the right.
function anything() {
  if (conn) conn.dispatch(messagename, arrayfromargs(arguments));
}

function bind(address, n) {
  if (String(address) === NODE + "/p0") bound = Number(n);
}

function paint() {
  const w = this.box.rect[2] - this.box.rect[0];
  const h = this.box.rect[3] - this.box.rect[1];
  mgraphics.set_source_rgba(0.1, 0.1, 0.1, 1);
  mgraphics.rectangle(0, 0, w, h);
  mgraphics.fill();

  const top = 24;
  const plot = h - top - 4;
  mgraphics.set_source_rgba(0.35, 0.75, 1, 1);
  for (let i = 0; i < N; i++) {
    const v = values[i];
    if (v < 0) continue;
    mgraphics.rectangle((i / (N - 1)) * (w - 3), top + (1 - v) * (plot - 2), 2, 2);
  }
  mgraphics.fill();

  mgraphics.set_source_rgba(1, 1, 1, 1);
  mgraphics.select_font_face("Arial");
  mgraphics.set_font_size(12);
  mgraphics.move_to(6, 16);
  mgraphics.show_text(
    valuesPerSecond + " values/s, " + groupsPerSecond + " groups/s" +
    (bound > 0 ? "" : ", waiting for the device (click expose)"));
}

function onresize() {
  mgraphics.redraw();
}
