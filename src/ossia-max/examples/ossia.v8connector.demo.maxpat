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
        "rect": [ 368.0, 158.0, 1062.0, 925.0 ],
        "showrootpatcherontab": 0,
        "showontab": 0,
        "boxes": [
            {
                "box": {
                    "fontface": 1,
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-2",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 20.0, 41.935483870967744, 233.06451612903226, 50.0 ],
                    "text": "ossia.v8connector demo: one tree, an ossia.parameter placed by hand and two v8ui that declare their parameters."
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 472.5806451612903, 92.4516129032258, 190.0, 22.0 ],
                    "text": "ossia.device v8connector_demo"
                }
            },
            {
                "box": {
                    "id": "obj-4",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 472.5806451612903, 61.806451612903224, 170.0, 22.0 ],
                    "text": "expose oscquery 7001 7002"
                }
            },
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-5",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 664.5806451612902, 45.80645161290323, 172.58064516129033, 54.0 ],
                    "text": "optional: publish the tree (OSC port, OSCQuery port). Both ports must be free."
                }
            },
            {
                "box": {
                    "id": "obj-6",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 472.5806451612903, 121.48387096774192, 80.0, 22.0 ],
                    "text": "print device"
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-7",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 121.7741935483871, 202.83400809716596, 21.0 ],
                    "text": "A parameter placed by hand:"
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-8",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 151.61290322580646, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "" ],
                    "patching_rect": [ 20.0, 181.4516129032258, 394.0, 22.0 ],
                    "text": "ossia.parameter manual/level @type float @min 0 @max 1 @default 0.5"
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-10",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 212.09677419354838, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-11",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 270.16129032258067, 734.6774193548387, 35.0 ],
                    "text": "Two different v8ui, each with its own ossia.v8connector, one parameter set. The left script declares gain, cutoff and voices under the node ui (add). The right script creates nothing: it binds to ui/gain, ui/cutoff and ui/voices, the parameters of the left one, and mirrors them."
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-12",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 23.387096774193548, 317.1774193548387, 304.03225806451616, 35.0 ],
                    "text": "scripting name ui_a, add mode: the script finds the object by that name, no cord to it"
                }
            },
            {
                "box": {
                    "color": [ 0.427450980392157, 0.843137254901961, 1.0, 1.0 ],
                    "id": "obj-13",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "" ],
                    "patching_rect": [ 20.0, 359.6774193548387, 180.0, 22.0 ],
                    "saved_attribute_attributes": {
                        "color": {
                            "expression": "themecolor.live_threshold_line_color"
                        }
                    },
                    "saved_newobj_attribute_attributes": {
                        "color": {
                            "expression": "themecolor.live_threshold_line_color"
                        }
                    },
                    "text": "ossia.v8connector ui",
                    "varname": "ui_a"
                }
            },
            {
                "box": {
                    "id": "obj-14",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 220.0, 359.6774193548387, 90.0, 22.0 ],
                    "text": "print ui_a"
                }
            },
            {
                "box": {
                    "border": 0,
                    "embed": 0,
                    "filename": "ossia.v8connector.demo.js",
                    "id": "obj-15",
                    "jsarguments": [ "ui_a" ],
                    "maxclass": "v8ui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 394.35483870967744, 300.0, 120.0 ],
                    "textfile": {
                        "filename": "ossia.v8connector.demo.js",
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
                    "id": "obj-16",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 349.19354838709677, 317.741935483871, 501.61290322580646, 35.0 ],
                    "text": "scripting name ui_b, bind mode: drag here, the left v8ui and the remotes follow; drag there, this one follows. Remove the left connector: this one waits and rebinds when it comes back."
                }
            },
            {
                "box": {
                    "color": [ 0.427450980392157, 0.843137254901961, 1.0, 1.0 ],
                    "id": "obj-17",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "" ],
                    "patching_rect": [ 349.19354838709677, 359.6774193548387, 180.0, 22.0 ],
                    "saved_attribute_attributes": {
                        "color": {
                            "expression": "themecolor.live_threshold_line_color"
                        }
                    },
                    "saved_newobj_attribute_attributes": {
                        "color": {
                            "expression": "themecolor.live_threshold_line_color"
                        }
                    },
                    "text": "ossia.v8connector mirror",
                    "varname": "ui_b"
                }
            },
            {
                "box": {
                    "id": "obj-18",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 549.1935483870968, 359.6774193548387, 90.0, 22.0 ],
                    "text": "print ui_b"
                }
            },
            {
                "box": {
                    "border": 0,
                    "embed": 0,
                    "filename": "ossia.v8connector.demo.js",
                    "id": "obj-19",
                    "jsarguments": [ "ui_b", "bind", "ui" ],
                    "maxclass": "v8ui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 349.19354838709677, 394.35483870967744, 300.0, 120.0 ],
                    "textfile": {
                        "filename": "ossia.v8connector.demo.js",
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
                    "id": "obj-20",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 543.5483870967741, 558.0, 35.0 ],
                    "text": "Other ossia objects listening to the same addresses. Drag a slider in either v8ui: the matching remote follows. Type a value here: both v8ui follow (the values come back through the connectors' left outlets)."
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-21",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 584.6774193548387, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-22",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "" ],
                    "patching_rect": [ 20.0, 615.3225806451613, 190.0, 22.0 ],
                    "text": "ossia.remote ui/gain"
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-23",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 20.0, 645.1612903225806, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-24",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 300.0, 584.6774193548387, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-25",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "" ],
                    "patching_rect": [ 300.0, 615.3225806451613, 190.0, 22.0 ],
                    "text": "ossia.remote ui/cutoff"
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-26",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 300.0, 645.1612903225806, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-27",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 580.0, 584.6774193548387, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-28",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "" ],
                    "patching_rect": [ 580.0, 615.3225806451613, 190.0, 22.0 ],
                    "text": "ossia.remote ui/voices"
                }
            },
            {
                "box": {
                    "format": 6,
                    "id": "obj-29",
                    "maxclass": "flonum",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 580.0, 645.1612903225806, 60.0, 22.0 ]
                }
            },
            {
                "box": {
                    "fontname": "Lato Regular",
                    "fontsize": 12.0,
                    "id": "obj-30",
                    "linecount": 3,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.274193548387075, 678.2258064516129, 401.45161290322585, 50.0 ],
                    "text": "A script's own set never comes back to it; it reaches the network, the remotes above and the other script. A declaration without @min and @max for a numeric type is refused: see the Max console, and refused <address> <reason> on the connector's left outlet.",
                    "textcolor": [ 1.0, 0.396078431372549, 0.0, 1.0 ]
                }
            },
            {
                "box": {
                    "background": 1,
                    "bgcolor": [ 0.92549, 0.815686, 0.101961, 1.0 ],
                    "fontname": "Arial Bold",
                    "hint": "",
                    "id": "obj-32",
                    "ignoreclick": 1,
                    "legacytextcolor": 1,
                    "maxclass": "textbutton",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 19.193548387096772, 270.16129032258067, 20.0, 20.0 ],
                    "rounded": 60.0,
                    "saved_attribute_attributes": {
                        "bgcolor": {
                            "expression": "themecolor.lesson_step_circle"
                        }
                    },
                    "text": "2",
                    "textcolor": [ 0.2, 0.2, 0.2, 1.0 ]
                }
            },
            {
                "box": {
                    "background": 1,
                    "bgcolor": [ 0.92549, 0.815686, 0.101961, 1.0 ],
                    "fontname": "Arial Bold",
                    "hint": "",
                    "id": "obj-93",
                    "ignoreclick": 1,
                    "legacytextcolor": 1,
                    "maxclass": "textbutton",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 19.354838709677427, 543.5483870967741, 20.0, 20.0 ],
                    "rounded": 60.0,
                    "saved_attribute_attributes": {
                        "bgcolor": {
                            "expression": "themecolor.lesson_step_circle"
                        }
                    },
                    "text": "3",
                    "textcolor": [ 0.2, 0.2, 0.2, 1.0 ]
                }
            },
            {
                "box": {
                    "background": 1,
                    "bgcolor": [ 0.92549, 0.815686, 0.101961, 1.0 ],
                    "fontname": "Arial Bold",
                    "hint": "",
                    "id": "obj-31",
                    "ignoreclick": 1,
                    "legacytextcolor": 1,
                    "maxclass": "textbutton",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "", "", "int" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 19.193548387096772, 122.58064516129032, 20.0, 20.0 ],
                    "rounded": 60.0,
                    "saved_attribute_attributes": {
                        "bgcolor": {
                            "expression": "themecolor.lesson_step_circle"
                        }
                    },
                    "text": "1",
                    "textcolor": [ 0.2, 0.2, 0.2, 1.0 ]
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-14", 0 ],
                    "source": [ "obj-13", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-15", 0 ],
                    "source": [ "obj-13", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-18", 0 ],
                    "source": [ "obj-17", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-19", 0 ],
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
                    "destination": [ "obj-23", 0 ],
                    "source": [ "obj-22", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-25", 0 ],
                    "source": [ "obj-24", 0 ]
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
                    "destination": [ "obj-28", 0 ],
                    "source": [ "obj-27", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-29", 0 ],
                    "source": [ "obj-28", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-6", 0 ],
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "midpoints": [ 482.0806451612903, 84.16129032258064, 482.0806451612903, 84.16129032258064 ],
                    "source": [ "obj-4", 0 ]
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
            }
        ],
        "autosave": 0
    }
}