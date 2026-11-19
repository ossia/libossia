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
        "rect": [ 368.0, 158.0, 760.0, 720.0 ],
        "showrootpatcherontab": 0,
        "showontab": 0,
        "boxes": [
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "fontface": 1,
                    "id": "obj-2",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 41.0, 700.0, 50.0 ],
                    "text": "ossia.v8connector load: an emitter sets 3000 leaves at each frame (load/p0 to load/p2999); a client in the same Max follows them by URL and draws them as a cloud of points. Compare @bundle 1 (the values of a frame in a few OSC bundles) and 0 (one message per value)."
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "patching_rect": [ 20.0, 100.0, 60.0, 22.0 ],
                    "text": "loadbang"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 128.0, 170.0, 22.0 ],
                    "text": "expose oscquery 7301 7302"
                }
            },
            {
                "box": {
                    "id": "obj-24",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "outlettype": [],
                    "patching_rect": [ 20.0, 156.0, 130.0, 22.0 ],
                    "text": "s load_demo_device"
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-6",
                    "linecount": 4,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 440.0, 100.0, 300.0, 64.0 ],
                    "text": "At load, the emitter's device exposes (OSC 7301, OSCQuery 7302). The client connects when the emitter sends declared: its 3000 leaves exist, the client reads the whole tree at once. Do not expose again: reopen the patcher instead."
                }
            },
            {
                "box": {
                    "id": "obj-7",
                    "maxclass": "toggle",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 195.0, 24.0, 24.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-8",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "outlettype": [],
                    "patching_rect": [ 20.0, 228.0, 110.0, 22.0 ],
                    "text": "s load_demo_run"
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-9",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 50.0, 197.0, 200.0, 21.0 ],
                    "text": "Run: 3000 values per frame"
                }
            },
            {
                "box": {
                    "id": "obj-10",
                    "maxclass": "toggle",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 260.0, 195.0, 24.0, 24.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-11",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "outlettype": [],
                    "patching_rect": [ 260.0, 228.0, 120.0, 22.0 ],
                    "text": "s load_demo_bundle"
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-12",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 290.0, 197.0, 300.0, 21.0 ],
                    "text": "@bundle of the emitter: 1 (bundles) or 0"
                }
            },
            {
                "box": {
                    "id": "obj-13",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "patching_rect": [ 600.0, 200.0, 60.0, 22.0 ],
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
                    "patching_rect": [ 600.0, 228.0, 30.0, 22.0 ],
                    "text": "1"
                }
            },
            {
                "box": {
                    "id": "obj-15",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 600.0, 120.0, 90.0, 22.0 ],
                    "text": "p emitter",
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
                        "rect": [ 100.0, 120.0, 560.0, 360.0 ],
                        "showontab": 0,
                        "boxes": [
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-1",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 20.0, 15.0, 520.0, 35.0 ],
                                    "text": "The emitter: an ossia.device of its own, and a connector whose 3000 leaves are set at each frame."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-2",
                                    "maxclass": "newobj",
                                    "numinlets": 0,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 20.0, 60.0, 130.0, 22.0 ],
                                    "text": "r load_demo_device"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-3",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 20.0, 90.0, 150.0, 22.0 ],
                                    "text": "ossia.device load_demo"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-4",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "outlettype": [],
                                    "patching_rect": [ 20.0, 120.0, 110.0, 22.0 ],
                                    "text": "print load_demo"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-5",
                                    "maxclass": "newobj",
                                    "numinlets": 0,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 260.0, 60.0, 110.0, 22.0 ],
                                    "text": "r load_demo_run"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-6",
                                    "maxclass": "newobj",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "bang" ],
                                    "patching_rect": [ 260.0, 90.0, 70.0, 22.0 ],
                                    "text": "qmetro 16"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 260.0, 210.0, 280.0, 22.0 ],
                                    "text": "v8 ossia.v8connector.load.emitter.js load_emitter 3000"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-8",
                                    "maxclass": "newobj",
                                    "numinlets": 0,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 20.0, 170.0, 130.0, 22.0 ],
                                    "text": "r load_demo_bundle"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-9",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 20.0, 200.0, 90.0, 22.0 ],
                                    "text": "prepend bundle"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-10",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "" ],
                                    "patching_rect": [ 20.0, 240.0, 150.0, 22.0 ],
                                    "text": "ossia.v8connector load",
                                    "varname": "load_emitter",
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
                                    "id": "obj-11",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "outlettype": [],
                                    "patching_rect": [ 190.0, 270.0, 100.0, 22.0 ],
                                    "text": "print load_emitter"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-13",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "outlettype": [],
                                    "patching_rect": [ 260.0, 240.0, 140.0, 22.0 ],
                                    "text": "s load_demo_declared"
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
                                    "patching_rect": [ 20.0, 300.0, 520.0, 50.0 ],
                                    "text": "The script reaches the connector by its scripting name, load_emitter; the connector's left outlet comes back into it for ready. It sends declared once its 3000 leaves exist on the device."
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [ "obj-3", 0 ],
                                    "source": [ "obj-2", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-4", 0 ],
                                    "source": [ "obj-3", 0 ]
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
                                    "destination": [ "obj-7", 0 ],
                                    "source": [ "obj-6", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-9", 0 ],
                                    "source": [ "obj-8", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-10", 0 ],
                                    "source": [ "obj-9", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-7", 0 ],
                                    "source": [ "obj-10", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-11", 0 ],
                                    "source": [ "obj-10", 1 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-13", 0 ],
                                    "source": [ "obj-7", 0 ]
                                }
                            }
                        ]
                    }
                }
            },
            {
                "box": {
                    "id": "obj-16",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 262.0, 140.0, 22.0 ],
                    "text": "r load_demo_declared"
                }
            },
            {
                "box": {
                    "id": "obj-25",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 2,
                    "outlettype": [ "bang", "bang" ],
                    "patching_rect": [ 170.0, 262.0, 70.0, 22.0 ],
                    "text": "onebang 1"
                }
            },
            {
                "box": {
                    "id": "obj-26",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 170.0, 290.0, 60.0, 22.0 ],
                    "text": "deferlow"
                }
            },
            {
                "box": {
                    "id": "obj-17",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 318.0, 230.0, 22.0 ],
                    "text": "connect oscquery ws://127.0.0.1:7302"
                }
            },
            {
                "box": {
                    "id": "obj-18",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 20.0, 348.0, 130.0, 22.0 ],
                    "text": "ossia.client load_view"
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-20",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 270.0, 284.0, 450.0, 35.0 ],
                    "text": "The receiver: an ossia.client connected by URL to the emitter's device once the emitter declared its leaves. deferlow sends connect after the patcher has loaded, when the device listens."
                }
            },
            {
                "box": {
                    "id": "obj-21",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "" ],
                    "patching_rect": [ 20.0, 390.0, 150.0, 22.0 ],
                    "text": "ossia.v8connector watch",
                    "varname": "load_receiver",
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
                    "border": 0,
                    "embed": 0,
                    "filename": "ossia.v8connector.load.receiver.js",
                    "id": "obj-22",
                    "jsarguments": [ "load_receiver", 3000, "load" ],
                    "maxclass": "v8ui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 425.0, 700.0, 260.0 ],
                    "textfile": {
                        "filename": "ossia.v8connector.load.receiver.js",
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
                    "id": "obj-23",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 190.0, 384.0, 530.0, 35.0 ],
                    "text": "Scripting name load_receiver: the v8ui binds load/p0 to load/p2999 through it; both outlets of the connector come back into the v8ui. Values/s and groups/s are counted by the v8ui."
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
                    "destination": [ "obj-24", 0 ],
                    "source": [ "obj-5", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-25", 0 ],
                    "source": [ "obj-16", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-26", 0 ],
                    "source": [ "obj-25", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-17", 0 ],
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-8", 0 ],
                    "source": [ "obj-7", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-11", 0 ],
                    "source": [ "obj-10", 0 ]
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
                    "destination": [ "obj-10", 0 ],
                    "source": [ "obj-14", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-18", 0 ],
                    "source": [ "obj-17", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-22", 0 ],
                    "source": [ "obj-21", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-22", 0 ],
                    "source": [ "obj-21", 1 ]
                }
            }
        ],
        "autosave": 0
    }
}
