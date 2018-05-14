// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_context.hpp"
#include <wobjectimpl.h>
W_GADGET_IMPL(ossia::qt::qml_val_type)
W_GADGET_IMPL(ossia::qt::qml_access_mode)
W_GADGET_IMPL(ossia::qt::qml_bounding_mode)
W_GADGET_IMPL(ossia::qt::qml_rep_filter)
W_GADGET_IMPL(ossia::qt::qml_duration)

namespace ossia::detail {
auto do_not_call_me()
{
  static volatile auto arr = {
    &ossia::qt::qml_val_type::staticMetaObject,
    &ossia::qt::qml_access_mode::staticMetaObject,
    &ossia::qt::qml_bounding_mode::staticMetaObject,
    &ossia::qt::qml_rep_filter::staticMetaObject,
    &ossia::qt::qml_duration::staticMetaObject
  };
}
}
