#pragma once
#include <ossia-config.hpp>
#if defined(OSSIA_EDITOR)
#include <ossia/editor/state/state_element.hpp>
#endif
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia-qt/value_metatypes.hpp>

#if defined(OSSIA_EDITOR)
Q_DECLARE_METATYPE(ossia::state)
Q_DECLARE_METATYPE(ossia::state_element)

W_REGISTER_ARGTYPE(ossia::state)
W_REGISTER_ARGTYPE(ossia::state_element)
#endif

Q_DECLARE_METATYPE(ossia::net::parameter_base*)
Q_DECLARE_METATYPE(ossia::net::node_base*)
Q_DECLARE_METATYPE(ossia::net::device_base*)
Q_DECLARE_METATYPE(ossia::net::protocol_base*)
Q_DECLARE_METATYPE(ossia::extended_attributes)

Q_DECLARE_METATYPE(ossia::traversal::path)
Q_DECLARE_METATYPE(ossia::access_mode)
Q_DECLARE_METATYPE(ossia::bounding_mode)
Q_DECLARE_METATYPE(ossia::unit_t)

W_REGISTER_ARGTYPE(ossia::net::parameter_base*)
W_REGISTER_ARGTYPE(ossia::net::node_base*)
W_REGISTER_ARGTYPE(ossia::net::device_base*)
W_REGISTER_ARGTYPE(ossia::net::protocol_base*)
W_REGISTER_ARGTYPE(ossia::extended_attributes)

W_REGISTER_ARGTYPE(ossia::traversal::path)
W_REGISTER_ARGTYPE(ossia::access_mode)
W_REGISTER_ARGTYPE(ossia::bounding_mode)
W_REGISTER_ARGTYPE(ossia::unit_t)

