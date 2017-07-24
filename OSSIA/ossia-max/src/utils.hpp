#pragma once
#include "ext.h"
#include "ext_obex.h"
#include "jpatcher_api.h"
#undef error
#undef post

#include "ossia_object_base.hpp"

namespace ossia
{
namespace max
{

/**
 * @brief find_peers: find object of the same class in the patcher
 * @param x : object we are looking for friend
 * @return true if we find one, false otherwise
 */
bool find_peers(t_object_base *x);

} // namespace max
} // namespace ossia
