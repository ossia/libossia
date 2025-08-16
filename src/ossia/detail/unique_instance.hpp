#pragma once

#include <libremidi/detail/memory.hpp>

namespace ossia
{
template <typename T, auto func>
using unique_handle = std::unique_ptr<T, libremidi::deleter<func>>;

template <typename T>
static constexpr auto unique_instance = libremidi::instance<T>;
}
