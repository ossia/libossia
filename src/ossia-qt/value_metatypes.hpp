#pragma once
#include <ossia/detail/config.hpp>
#include <wobjectdefs.h>
#include <ossia/network/value/value.hpp>
#include <QMetaType>

Q_DECLARE_METATYPE(ossia::vec2f)
Q_DECLARE_METATYPE(ossia::vec3f)
Q_DECLARE_METATYPE(ossia::vec4f)
Q_DECLARE_METATYPE(ossia::impulse)
Q_DECLARE_METATYPE(ossia::value)
Q_DECLARE_METATYPE(ossia::val_type)

W_REGISTER_ARGTYPE(ossia::vec2f)
W_REGISTER_ARGTYPE(ossia::vec3f)
W_REGISTER_ARGTYPE(ossia::vec4f)
W_REGISTER_ARGTYPE(ossia::impulse)
W_REGISTER_ARGTYPE(ossia::value)
W_REGISTER_ARGTYPE(ossia::val_type)
