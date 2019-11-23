#ifndef OSSIA_PREFIX_HEADER
#define OSSIA_PREFIX_HEADER
#pragma warning(push, 0) // MSVC
#pragma GCC system_header
#pragma clang system_header

////////
#include <ossia/detail/config.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/value.hpp>

#include <tsl/hopscotch_map.h>

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#endif
