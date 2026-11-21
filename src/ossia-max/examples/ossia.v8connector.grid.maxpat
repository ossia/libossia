{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 2,
            "revision": 0,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 368.0, 158.0, 700.0, 680.0 ],
        "showrootpatcherontab": 0,
        "showontab": 0,
        "boxes": [
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "fontface": 1,
                    "id": "obj-2",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 41.0, 640.0, 35.0 ],
                    "text": "ossia.v8connector grid: a click on a cell of the v8ui declares the parameter grid/cell/<x>/<y> (float, 0 to 1), a second click removes it. The jweb shows the tree served by the device, live."
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 95.0, 170.0, 22.0 ],
                    "text": "expose oscquery 7101 7102"
                }
            },
            {
                "box": {
                    "id": "obj-4",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 200.0, 95.0, 35.0, 22.0 ],
                    "text": "stop"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 128.0, 170.0, 22.0 ],
                    "text": "ossia.device grid_example"
                }
            },
            {
                "box": {
                    "id": "obj-6",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "outlettype": [],
                    "patching_rect": [ 20.0, 158.0, 80.0, 22.0 ],
                    "text": "print device"
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-7",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 250.0, 89.0, 410.0, 64.0 ],
                    "text": "Click expose once: it publishes the tree, OSC port 7101 and OSCQuery port 7102. Exposing again on ports already in use fails and can crash Max: send stop to the device before exposing again."
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-8",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 200.0, 280.0, 50.0 ],
                    "text": "Scripting name grid: the script finds the connector by that name, no cord to it. Its left outlet comes back into the v8ui."
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "" ],
                    "patching_rect": [ 20.0, 258.0, 150.0, 22.0 ],
                    "text": "ossia.v8connector grid",
                    "varname": "grid",
                    "color": [ 0.427450980392157, 0.843137254901961, 1.0, 1.0 ],
                    "saved_attribute_attributes": {
                        "color": {
                            "expression": "themecolor.live_threshold_line_color"
                        }
                    },
                    "saved_newobj_attribute_attributes": {
                        "color": {
                            "expression": "themecolor.live_threshold_line_color"
                        }
                    }
                }
            },
            {
                "box": {
                    "id": "obj-10",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "outlettype": [],
                    "patching_rect": [ 190.0, 258.0, 70.0, 22.0 ],
                    "text": "print grid"
                }
            },
            {
                "box": {
                    "border": 0,
                    "embed": 0,
                    "filename": "ossia.v8connector.grid.js",
                    "id": "obj-11",
                    "jsarguments": [ "grid", 8 ],
                    "maxclass": "v8ui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 292.0, 240.0, 240.0 ],
                    "textfile": {
                        "filename": "ossia.v8connector.grid.js",
                        "flags": 0,
                        "embed": 0,
                        "autowatch": 1
                    }
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-12",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 320.0, 200.0, 340.0, 50.0 ],
                    "text": "The tree served on port 7102, read over HTTP then kept up to date by the server's WebSocket (PATH_ADDED, PATH_REMOVED). It waits until the device is exposed."
                }
            },
            {
                "box": {
                    "id": "obj-13",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "patching_rect": [ 320.0, 258.0, 60.0, 22.0 ],
                    "text": "loadbang"
                }
            },
            {
                "box": {
                    "id": "obj-14",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 390.0, 258.0, 230.0, 22.0 ],
                    "text": "readfile ossia.v8connector.grid.html"
                }
            },
            {
                "box": {
                    "disablefind": 0,
                    "id": "obj-15",
                    "maxclass": "jweb",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 320.0, 292.0, 340.0, 340.0 ],
                    "rendermode": 2
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-16",
                    "linecount": 4,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 545.0, 280.0, 64.0 ],
                    "text": "The cells are declared under grid, the node of the connector: grid/cell/3/4. The helper replays them when the connector is recreated."
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-5", 0 ],
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-5", 0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-6", 0 ],
                    "source": [ "obj-5", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-11", 0 ],
                    "source": [ "obj-9", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 0 ],
                    "source": [ "obj-9", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-14", 0 ],
                    "source": [ "obj-13", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-15", 0 ],
                    "source": [ "obj-14", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}
