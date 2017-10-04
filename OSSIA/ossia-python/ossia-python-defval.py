import ossia_python as ossia
local_device = ossia.LocalDevice("newDevice")

bool_node = local_device.add_node("/foo")
bool_node.default_value = True

print(str(bool_node.default_value))
