#pragma once
#include <ossia/detail/config.hpp>

#include <string_view>

namespace ossia::net
{
/**
 * @brief Indicates where nodes relating to a pattern match are to be searched
 */
enum class address_scope
{
  relative = 0, //! The address begins with '//', it can start from anywhere
  absolute,     //! The address is a standard OSC one: /foo/bar
  global        //! Like absolute, but the address has a device specified:
                //! device:/foo.
};

/**
 * @brief get_address_scope: return address scope (relative, absolute or
 * globale)
 * @param addr: the address to process
 * @return the scope
 */
OSSIA_EXPORT
ossia::net::address_scope get_address_scope(std::string_view addr);
}
