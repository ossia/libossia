#pragma once
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia-qt/value_metatypes.hpp>

Q_DECLARE_METATYPE(ossia::state)
Q_DECLARE_METATYPE(ossia::custom_state)
Q_DECLARE_METATYPE(ossia::state_element)

Q_DECLARE_METATYPE(ossia::net::parameter_base*)
Q_DECLARE_METATYPE(ossia::net::node_base*)
Q_DECLARE_METATYPE(ossia::net::device_base*)
Q_DECLARE_METATYPE(ossia::net::protocol_base*)
Q_DECLARE_METATYPE(ossia::extended_attributes)

Q_DECLARE_METATYPE(ossia::traversal::path)
