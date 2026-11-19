// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// v8ui grid for ossia.v8connector: each cell the user turns on is a parameter.
// A click on an empty cell declares cell/<x>/<y> (float, 0 to 1) through the
// helper; a second click removes it. The script reaches the connector by its
// scripting name, without a patch cord; the connector's left outlet comes back
// into this v8ui for the values changed elsewhere, "ready" and "refused".
//
// Patch: [ossia.v8connector grid] with the scripting name "grid", its left
// outlet into [v8ui ossia.v8connector.grid.js grid 8].
// Arguments of the v8ui, after the script name:
//   1. scripting name of the connector (default "grid")
//   2. number of cells per side (default 8)

let OssiaConnector = null;
try {
  OssiaConnector = require("ossia.v8connector");
} catch (e) {
  error("ossia.v8connector.js not found: it lives in the javascript folder of the ossia package\n");
}

const CONNECTOR = jsarguments.length > 1 ? String(jsarguments[1]) : "grid";
const SIZE = jsarguments.length > 2 ? Math.max(1, Math.floor(Number(jsarguments[2]))) : 8;
const grid = OssiaConnector ? new OssiaConnector(CONNECTOR) : null;

// Cells turned on: address -> value. The helper keeps the declarations and
// replays them when the connector sends "ready" (connector deleted and
// recreated, patcher reloaded): the cells come back by themselves.
const cells = new Map();

mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;

function cellAddress(x, y) {
  return "cell/" + x + "/" + y;
}

function toggle(x, y) {
  if (!grid) return;
  const address = cellAddress(x, y);
  if (cells.has(address)) {
    cells.delete(address);
    grid.off(address);
    grid.remove(address);
  } else {
    cells.set(address, 0);
    grid.on(address, (v) => { cells.set(address, v); mgraphics.redraw(); });
    grid.add(address, { type: "float", min: 0, max: 1, default: 0 });
  }
  mgraphics.redraw();
}

// Values changed by another source, "ready" and "refused" arrive from the
// connector's left outlet.
function anything() {
  if (grid) grid.dispatch(messagename, arrayfromargs(arguments));
}

function onclick(x, y) {
  const w = this.box.rect[2] - this.box.rect[0];
  const h = this.box.rect[3] - this.box.rect[1];
  const cx = Math.floor((x / w) * SIZE);
  const cy = Math.floor((y / h) * SIZE);
  if (cx < 0 || cy < 0 || cx >= SIZE || cy >= SIZE) return;
  toggle(cx, cy);
}

function paint() {
  const w = this.box.rect[2] - this.box.rect[0];
  const h = this.box.rect[3] - this.box.rect[1];
  const cw = w / SIZE;
  const ch = h / SIZE;
  mgraphics.set_source_rgba(0.12, 0.12, 0.12, 1);
  mgraphics.rectangle(0, 0, w, h);
  mgraphics.fill();
  for (let x = 0; x < SIZE; x++) {
    for (let y = 0; y < SIZE; y++) {
      const address = cellAddress(x, y);
      if (cells.has(address)) {
        // brighter with the value: a remote client can drive the cell
        const v = Math.max(0, Math.min(1, Number(cells.get(address)) || 0));
        mgraphics.set_source_rgba(0.2, 0.55, 0.85, 0.45 + 0.55 * v);
      } else {
        mgraphics.set_source_rgba(0.22, 0.22, 0.22, 1);
      }
      mgraphics.rectangle(x * cw + 1, y * ch + 1, cw - 2, ch - 2);
      mgraphics.fill();
    }
  }
}

function onresize() {
  mgraphics.redraw();
}
