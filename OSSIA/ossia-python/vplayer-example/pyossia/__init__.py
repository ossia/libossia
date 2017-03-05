#! /usr/bin/env python
# -*- coding: utf-8 -*-


# this is here fow now, to simplify development
# it creates a LocalDevice at pyossia import
import ossia_python as ossia

ossia_device = ossia.LocalDevice('PyOssia Device')
ossia_device.create_oscquery_server(3456, 5678)