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
  parse_error(std::string e):
    std::runtime_error(std::move(e))
  {

  }
};

/**
 * @brief The connection_error class
 *
 * Means that a connection could not be established.
 */
struct OSSIA_EXPORT connection_error:
    public std::runtime_error
{
  connection_error(std::string e):
    std::runtime_error(std::move(e))
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
  invalid_node_error(std::string e):
    std::logic_error(std::move(e))
  {

  }
};


}
