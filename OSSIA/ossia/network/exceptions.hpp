#pragma once
#include <ossia/editor/exceptions.hpp>

namespace ossia
{

/**
 * @brief The parse_error class
 *
 * Means that there was a problem
 * while parsing a network message.
 */
struct OSSIA_EXPORT parse_error :
    public std::runtime_error
{
  parse_error(const char* e):
    std::runtime_error(e)
  {

  }
};


/**
 * @brief The invalid_node_error class
 *
 * Means that there an incorrect node
 * was passed.
 * For instance a node without an address
 * when an address was required.
 */
struct OSSIA_EXPORT invalid_node_error :
    public std::logic_error
{
  invalid_node_error(const char* e):
    std::logic_error(e)
  {

  }
};


}
