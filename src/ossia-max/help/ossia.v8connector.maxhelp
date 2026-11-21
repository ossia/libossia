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
        "rect": [ 42.0, 111.0, 982.0, 862.0 ],
        "showrootpatcherontab": 0,
        "showontab": 0,
        "boxes": [
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 165.0, 172.0, 200.0, 22.0 ],
                    "text": "ossia.device v8connector_maxhelp"
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
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
                        "rect": [ 42.0, 137.0, 982.0, 836.0 ],
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "helpdetails.js",
                                    "id": "obj-1",
                                    "ignoreclick": 1,
                                    "jsarguments": [ "ossia.v8connector" ],
                                    "maxclass": "jsui",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 8.0, 7.0, 674.0, 142.0 ]
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-2",
                                    "linecount": 4,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 12.5, 165.0, 420.0, 64.0 ],
                                    "text": "ossia.v8connector creates a node of the ossia tree, like ossia.model, but its parameters are declared by messages instead of ossia.parameter objects. The usual sender is a v8 or v8ui script (see the Script tab); the messages below do the same thing by hand."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-3",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 12.5, 240.0, 570.0, 22.0 ],
                                    "text": "add gain @type float @min -60 @max 6 @unit gain.db @default -12 @clip both @description \"main gain\""
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-4",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 12.5, 270.0, 290.0, 22.0 ],
                                    "text": "add voices @type int @min 1 @max 16 @default 4"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-5",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 12.5, 300.0, 202.0, 22.0 ],
                                    "text": "add cutoff @type float @unit time.hz"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-6",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 226.5, 293.5, 230.0, 35.0 ],
                                    "text": "refused: a numeric type needs @min and @max (or @range)"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 11.0, 336.0, 346.0, 22.0 ],
                                    "text": "add mode @type string @range sine saw square @default sine"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-8",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 21.0, 363.0, 364.0, 35.0 ],
                                    "text": "set pushes a value; values leave bounded to the declared range (@clip both here), as every ossia object does:"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-9",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 12.5, 400.0, 60.0, 22.0 ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-10",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 12.5, 430.0, 80.0, 22.0 ],
                                    "text": "set gain $1"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-11",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 130.0, 430.0, 90.0, 22.0 ],
                                    "text": "remove voices"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-12",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 230.0, 430.0, 40.0, 22.0 ],
                                    "text": "clear"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-13",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 290.0, 430.0, 75.0, 22.0 ],
                                    "text": "namespace"
                                }
                            },
                            {
                                "box": {
                                    "color": [ 0.427450980392157, 0.843137254901961, 1.0, 1.0 ],
                                    "id": "obj-14",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "" ],
                                    "patching_rect": [ 12.5, 480.0, 343.0, 22.0 ],
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
                                    "text": "ossia.v8connector synth @description \"declared by messages\""
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-15",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 12.5, 520.0, 85.0, 22.0 ],
                                    "text": "print values"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-16",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 336.5, 520.0, 85.0, 22.0 ],
                                    "text": "print status"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-17",
                                    "linecount": 5,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 12.5, 560.0, 556.0, 78.0 ],
                                    "text": "Left outlet: <address> <value> for every value changed by another source (network, other ossia objects), bounded to the parameter's range then filtered by it (mute, enable, repetitions) like the outlet of any ossia object; a set sent here comes back only if the bounded value differs from the one sent. 'ready' once the object has registered. Right outlet: address, add, remove, clear, bind, unbind and errors, and the replies to namespace and get. Every refusal also comes out here: refused <address> <reason>."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-18",
                                    "linecount": 4,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 12.5, 646.0, 556.0, 64.0 ],
                                    "text": "The node registers like ossia.model: under the nearest ossia.model, ossia.device or ossia.client in the patcher hierarchy (here the ossia.device of the parent patcher), otherwise under the global device. It is created by the first add and stays, even empty, until the object is deleted; an instance that only binds creates no node."
                                }
                            },
                            {
                                "box": {
                                    "fontface": 1,
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-19",
                                    "linecount": 1,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 12.5, 718.0, 556.0, 20.0 ],
                                    "text": "Two instances with the same name become synth and synth.1, like two ossia.model."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-20",
                                    "linecount": 4,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 12.5, 746.0, 556.0, 64.0 ],
                                    "text": "Initialization follows ossia-max: the default of a leaf is stored quietly, then comes out of the left outlet once the leaf exists; bind outputs the value it finds as soon as it reaches the parameter. @priority is stored on the nodes but does not order these outputs: ossia-max orders the outputs of ossia.parameter and ossia.remote only."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-21",
                                    "linecount": 4,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 604.0, 165.0, 360.0, 64.0 ],
                                    "text": "Every other ossia object sees the same tree. This remote listens to synth/gain: a set from the connector reaches it, and a value typed here reaches the connector's left outlet (print values). Add gain first."
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-22",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 604.0, 240.0, 60.0, 22.0 ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-23",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 3,
                                    "outlettype": [ "", "", "" ],
                                    "patching_rect": [ 604.0, 270.0, 170.0, 22.0 ],
                                    "text": "ossia.remote synth/gain"
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
                                    "patching_rect": [ 604.0, 300.0, 60.0, 22.0 ]
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-25",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 604.0, 342.0, 360.0, 35.0 ],
                                    "text": "An ossia.parameter placed by hand lives next to the declared parameters, in the same tree:"
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
                                    "patching_rect": [ 604.0, 382.0, 60.0, 22.0 ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-27",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "" ],
                                    "patching_rect": [ 604.0, 412.0, 366.0, 22.0 ],
                                    "text": "ossia.parameter manual @type float @min 0 @max 1 @default 0.5"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-28",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 604.0, 452.0, 60.0, 22.0 ]
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-29",
                                    "linecount": 5,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 580.0, 530.0, 380.0, 78.0 ],
                                    "text": "Attributes of add use the names of ossia.parameter: @type (required), @min, @max, @range, @unit, @default, @clip, @mode, @description, @tags, @priority, @rate, @critical, @invisible, @mute, @repetitions, @enable, @recall_safe. @clip none is the free mode. A unit imposes its type: @type vec4f @unit color.rgb is refused."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-30",
                                    "linecount": 5,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 580.0, 618.0, 380.0, 78.0 ],
                                    "text": "bind follows a parameter that exists elsewhere, without creating anything: here the ossia.parameter above, addressed like an ossia.remote would (relative to the device). set pushes to it with the owner's type and range; its changes come back on the left outlet under the bound address."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-31",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 580.0, 706.0, 80.0, 22.0 ],
                                    "text": "bind manual"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-32",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 670.0, 706.0, 90.0, 22.0 ],
                                    "text": "unbind manual"
                                }
                            },
                            {
                                "box": {
                                    "format": 6,
                                    "id": "obj-33",
                                    "maxclass": "flonum",
                                    "numinlets": 1,
                                    "numoutlets": 2,
                                    "outlettype": [ "", "bang" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 780.0, 706.0, 50.0, 22.0 ]
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-34",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 780.0, 736.0, 100.0, 22.0 ],
                                    "text": "set manual $1"
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 22.0, 453.0, 22.0, 453.0 ],
                                    "source": [ "obj-10", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 139.5, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-11", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 239.5, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-12", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 299.5, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-13", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-15", 0 ],
                                    "source": [ "obj-14", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-16", 0 ],
                                    "source": [ "obj-14", 1 ]
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
                                    "destination": [ "obj-24", 0 ],
                                    "source": [ "obj-23", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-27", 0 ],
                                    "source": [ "obj-26", 0 ]
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
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 22.0, 264.0, 9.0, 264.0, 9.0, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-3", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 589.5, 820.0, 9.0, 820.0, 9.0, 477.0, 22.0, 477.0 ],
                                    "source": [ "obj-31", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 679.5, 820.0, 9.0, 820.0, 9.0, 477.0, 22.0, 477.0 ],
                                    "source": [ "obj-32", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-34", 0 ],
                                    "source": [ "obj-33", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 789.5, 820.0, 9.0, 820.0, 9.0, 477.0, 22.0, 477.0 ],
                                    "source": [ "obj-34", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 22.0, 294.0, 9.0, 294.0, 9.0, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-4", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 22.0, 336.0, 9.0, 336.0, 9.0, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-5", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-14", 0 ],
                                    "midpoints": [ 20.5, 372.0, 9.0, 372.0, 9.0, 465.0, 22.0, 465.0 ],
                                    "source": [ "obj-7", 0 ]
                                }
                            },
                            {
                                "patchline": {
                                    "destination": [ "obj-10", 0 ],
                                    "source": [ "obj-9", 0 ]
                                }
                            }
                        ]
                    },
                    "patching_rect": [ 165.0, 242.0, 50.0, 22.0 ],
                    "text": "p Basic"
                }
            },
            {
                "box": {
                    "id": "obj-4",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
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
                        "rect": [ 0.0, 26.0, 982.0, 836.0 ],
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "helpdetails.js",
                                    "id": "obj-1",
                                    "ignoreclick": 1,
                                    "jsarguments": [ "ossia.v8connector" ],
                                    "maxclass": "jsui",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 8.0, 7.0, 674.0, 142.0 ]
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-2",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 165.0, 656.0, 50.0 ],
                                    "text": "From a v8 or v8ui script, the helper javascript/ossia.v8connector.js reaches the object through its scripting name (varname in the inspector), without a patch cord. One cord brings the values back: the left outlet of the object into the inlet of the script."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-3",
                                    "linecount": 15,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 225.0, 800.0, 222.0 ],
                                    "text": "const OssiaConnector = require(\"ossia.v8connector\");\nconst synth = new OssiaConnector(\"synth\");   // scripting name of [ossia.v8connector synth]\n\nfunction loadbang() {\n  synth.declare({\n    gain:   { type: \"float\", min: -60, max: 6, unit: \"gain.db\", default: -12 },\n    cutoff: { type: \"float\", range: [20, 20000], unit: \"time.hz\" },\n    voices: { type: \"int\", min: 1, max: 16, default: 4 },\n  });\n  synth.on(\"gain\", (v) => { gain = v; });\n}\nfunction anything() { synth.dispatch(messagename, arrayfromargs(arguments)); }\n\nsynth.set(\"gain\", -6);      // to the network and to the other ossia objects, not back to this script\nsynth.remove(\"voices\");"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-4",
                                    "linecount": 5,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 500.0, 666.0, 78.0 ],
                                    "text": "Declarations are kept by the object: add can be sent before the object has registered (for instance from the script's loadbang). set is refused until the node exists; the right outlet sends 'address <global address>' when it does. The left outlet sends 'ready' once the object has registered: the helper then replays its declarations, so an object deleted and recreated (undo, paste) gets them again. Declaring the same leaf twice is accepted. A numeric type without @min and @max (or @range) is refused."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-5",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 580.0, 800.0, 35.0 ],
                                    "text": "The demo patcher ossia.v8connector.demo.maxpat (examples folder of the package) mixes two v8ui that declare through the object with an ossia.parameter placed by hand, in one tree."
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-6",
                                    "maxclass": "message",
                                    "numinlets": 2,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 30.0, 625.0, 250.0, 22.0 ],
                                    "text": "load ossia.v8connector.demo.maxpat"
                                }
                            },
                            {
                                "box": {
                                    "id": "obj-7",
                                    "maxclass": "newobj",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "patching_rect": [ 30.0, 655.0, 55.0, 22.0 ],
                                    "text": "pcontrol"
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-8",
                                    "linecount": 10,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 685.0, 900.0, 137.0 ],
                                    "text": "Recipe, add and remove from a script: a v8ui grid where a click creates cell/x/y and a second click removes it. The helper sends add and remove itself, through the scripting name; the left outlet is patched into the script only to receive values, ready and refused.\nconst grid = new OssiaConnector(\"grid\"), cells = new Set();   // [ossia.v8connector grid]\nfunction toggle(x, y) {\n  const a = \"cell/\" + x + \"/\" + y;\n  if (cells.has(a)) { cells.delete(a); grid.remove(a); }   // sends: remove cell/3/4\n  else { cells.add(a); grid.add(a, { type: \"float\", min: 0, max: 1, default: 0 }); }   // sends: add cell/3/4 @type float @min 0 @max 1 @default 0\n}\nOn ready (object deleted and recreated), the helper replays the cells still declared, not the removed ones."
                                }
                            }
                        ],
                        "lines": [
                            {
                                "patchline": {
                                    "destination": [ "obj-7", 0 ],
                                    "source": [ "obj-6", 0 ]
                                }
                            }
                        ]
                    },
                    "patching_rect": [ 230.0, 242.0, 50.0, 22.0 ],
                    "text": "p Script"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "newobj",
                    "numinlets": 0,
                    "numoutlets": 0,
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
                        "rect": [ 0.0, 26.0, 982.0, 836.0 ],
                        "showontab": 1,
                        "boxes": [
                            {
                                "box": {
                                    "border": 0,
                                    "filename": "helpdetails.js",
                                    "id": "obj-1",
                                    "ignoreclick": 1,
                                    "jsarguments": [ "ossia.v8connector" ],
                                    "maxclass": "jsui",
                                    "numinlets": 1,
                                    "numoutlets": 1,
                                    "outlettype": [ "" ],
                                    "parameter_enable": 0,
                                    "patching_rect": [ 8.0, 7.0, 674.0, 142.0 ]
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-3",
                                    "linecount": 3,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 200.0, 570.0, 50.0 ],
                                    "text": "@priority is stored on the node but does not order the outputs of ossia.v8connector: ossia-max orders the outputs of ossia.parameter and ossia.remote only. Defaults and bound values come out in declaration order."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-4",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 260.0, 685.0, 35.0 ],
                                    "text": "An ossia.model (or ossia.view) added to the patcher after the object has registered does not take it under its node: send 'address <name>' to the object, or reload the patcher. Models present when the object registers are followed when they are renamed or removed afterwards."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-5",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 320.0, 685.0, 35.0 ],
                                    "text": "'ready' and 'refused' are reserved addresses: the object sends ready on its left outlet once registered (the helper replays its declarations), and refused <address> <reason> for every refusal. No parameter can be named ready or refused."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-6",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 370.0, 685.0, 35.0 ],
                                    "text": "Inside one instance, an address is either declared by add or followed by bind, not both; two instances with the same name become name and name.1, like two ossia.model."
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
                                    "patching_rect": [ 30.0, 420.0, 685.0, 50.0 ],
                                    "text": "Values leave bounded to the parameter's range then filtered (mute, enable, repetitions), like the outlet of every ossia object; the parameter itself stores what it was given, like ossia.parameter. With @bundle 1 (the default), the values set during a tick leave together at its end, in OSC bundles; @bundle 0 sends one message per value."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-8",
                                    "linecount": 2,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 470.0, 638.0, 35.0 ],
                                    "text": "The OSCQuery server is the one of ossia.device."
                                }
                            },
                            {
                                "box": {
                                    "fontname": "Lato Regular",
                                    "fontsize": 12.0,
                                    "id": "obj-9",
                                    "linecount": 12,
                                    "maxclass": "comment",
                                    "numinlets": 1,
                                    "numoutlets": 0,
                                    "patching_rect": [ 30.0, 520.0, 685.0, 180.0 ],
                                    "text": "Who owns a parameter: an instance removes only what it declared. remove cell/3/4 sent to another instance is refused there (refused cell/3/4 unknown address 'cell/3/4') and the parameter stays: its owner replays its declarations on every ready, so a removal by another instance would not last. To act on a parameter declared elsewhere, bind it and set it.\n\nThe name argument is optional. [ossia.v8connector synth] names the node that holds the parameters of the instance, under the enclosing ossia.model or ossia.device: add gain creates synth/gain, and a second synth in the same place becomes synth.1. [ossia.v8connector] puts add gain directly under the enclosing model or device, next to the ossia.parameter objects; an address already taken is refused (refused gain already exists), never renamed, where an ossia.parameter would become gain.1. bind does not go through the instance: its address is relative to the enclosing model or device, as for ossia.remote."
                                }
                            }
                        ],
                        "lines": []
                    },
                    "patching_rect": [ 295.0, 242.0, 55.0, 22.0 ],
                    "text": "p Limits"
                }
            }
        ],
        "lines": [],
        "autosave": 0
    }
}