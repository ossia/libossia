// v8ui demo for ossia.v8connector: three parameters drawn as sliders.
// The state lives in this script; the connector only puts it in the ossia tree.
//
// Patch: [ossia.v8connector <name>] with a scripting name, its left outlet into this v8ui.
// Arguments of the v8ui, after the script name:
//   1. scripting name of the connector            [v8ui ossia.v8connector.demo.js ui_a]
//   2. "add" (default) declares the parameters under the connector's node;
//      "bind" follows parameters that exist elsewhere, without creating anything
//   3. in bind mode, the node of the owner         [v8ui ossia.v8connector.demo.js ui_b bind ui]

let OssiaConnector = null;
try {
  OssiaConnector = require("ossia.v8connector");
} catch (e) {
  error("ossia.v8connector.js not found: it lives in the javascript folder of the ossia package\n");
}

const CONNECTOR = jsarguments.length > 1 ? String(jsarguments[1]) : "ui";
const MODE = jsarguments.length > 2 ? String(jsarguments[2]) : "add";
const OWNER = jsarguments.length > 3 ? String(jsarguments[3]) + "/" : "";
const ossia = OssiaConnector ? new OssiaConnector(CONNECTOR) : null;

const params = [
  { address: "gain", type: "float", min: -60, max: 6, unit: "gain.db", value: -12, label: "gain (dB)" },
  { address: "cutoff", type: "float", min: 20, max: 20000, unit: "time.hz", value: 1000, label: "cutoff (Hz)" },
  { address: "voices", type: "int", min: 1, max: 16, value: 4, label: "voices" },
];
// The address used with set and on: relative to the connector's node in add
// mode, relative to the enclosing model or device in bind mode (like ossia.remote).
for (const p of params) p.key = (MODE === "bind" ? OWNER : "") + p.address;

mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;

function declareAll() {
  if (!ossia) return;
  const spec = {};
  for (const p of params) {
    ossia.on(p.key, (v) => { p.value = v; mgraphics.redraw(); });
    if (MODE === "bind") {
      ossia.bind(p.key);
    } else {
      spec[p.address] = { type: p.type, min: p.min, max: p.max, default: p.value, clip: "both" };
      if (p.unit) spec[p.address].unit = p.unit;
    }
  }
  if (MODE !== "bind") ossia.declare(spec);
}

// Values changed by another source arrive from the connector's left outlet as <address> <value>.
function anything() {
  if (ossia) ossia.dispatch(messagename, arrayfromargs(arguments));
}

function bang() { declareAll(); }
function loadbang() { declareAll(); }

function paint() {
  const w = this.box.rect[2] - this.box.rect[0];
  const h = this.box.rect[3] - this.box.rect[1];
  const rowH = h / params.length;
  mgraphics.set_source_rgba(0.12, 0.12, 0.12, 1);
  mgraphics.rectangle(0, 0, w, h);
  mgraphics.fill();
  params.forEach((p, i) => {
    const y = i * rowH;
    const frac = (p.value - p.min) / (p.max - p.min);
    if (MODE === "bind") mgraphics.set_source_rgba(0.85, 0.55, 0.2, 1);
    else mgraphics.set_source_rgba(0.2, 0.55, 0.85, 1);
    mgraphics.rectangle(0, y + 4, Math.max(0, Math.min(1, frac)) * w, rowH - 8);
    mgraphics.fill();
    mgraphics.set_source_rgba(1, 1, 1, 1);
    mgraphics.select_font_face("Arial");
    mgraphics.set_font_size(12);
    mgraphics.move_to(6, y + rowH / 2 + 4);
    mgraphics.show_text(CONNECTOR + " " + MODE + " " + p.key + ": " + (p.type === "int" ? p.value : p.value.toFixed(2)));
  });
}

function setFromMouse(x, y) {
  const w = this.box.rect[2] - this.box.rect[0];
  const h = this.box.rect[3] - this.box.rect[1];
  const i = Math.max(0, Math.min(params.length - 1, Math.floor(y / (h / params.length))));
  const p = params[i];
  let v = p.min + Math.max(0, Math.min(1, x / w)) * (p.max - p.min);
  if (p.type === "int") v = Math.round(v);
  p.value = v;
  if (ossia) ossia.set(p.key, v);
  mgraphics.redraw();
}

function onclick(x, y) { setFromMouse.call(this, x, y); }
function ondrag(x, y) { setFromMouse.call(this, x, y); }
