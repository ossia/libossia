#pragma once
#include <ossia-qt/value_metatypes.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/path.hpp>

Q_DECLARE_METATYPE(ossia::state)
Q_DECLARE_METATYPE(ossia::custom_state)
Q_DECLARE_METATYPE(ossia::state_element)

Q_DECLARE_METATYPE(ossia::net::address_base*)
Q_DECLARE_METATYPE(ossia::net::node_base*)
Q_DECLARE_METATYPE(ossia::net::device_base*)
Q_DECLARE_METATYPE(ossia::net::protocol_base*)
Q_DECLARE_METATYPE(ossia::extended_attributes)

Q_DECLARE_METATYPE(ossia::traversal::path)
