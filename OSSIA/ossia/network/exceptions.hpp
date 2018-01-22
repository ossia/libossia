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
struct OSSIA_EXPORT parse_error final : public std::runtime_error
{
  parse_error(std::string e) : std::runtime_error(std::move(e))
  {
  }
  ~parse_error() override;
};

/**
 * @brief The connection_error class
 *
 * Means that a connection could not be established.
 */
struct OSSIA_EXPORT connection_error final : public std::runtime_error
{
  connection_error(std::string e) : std::runtime_error(std::move(e))
  {
  }
  ~connection_error() override;
};

/**
 * @brief The invalid_node_error class
 *
 * Means that there an incorrect node
 * was passed.
 * For instance a node without a parameter
 * when a parameter was required.
 */
struct OSSIA_EXPORT invalid_node_error final : public std::logic_error
{
  invalid_node_error(std::string e) : std::logic_error(std::move(e))
  {
  }
  ~invalid_node_error() override;
};

/**
 * @brief Used when a requested node could not be found.
 */
struct OSSIA_EXPORT node_not_found_error final : public std::runtime_error
{
  node_not_found_error(std::string e) : std::runtime_error(std::move(e))
  {
  }
  ~node_not_found_error() override;
};

/**
 * @brief Used when a bad network request is done on a local server.
 */
struct OSSIA_EXPORT bad_request_error final : public std::runtime_error
{
  bad_request_error(std::string e) : std::runtime_error(std::move(e))
  {
  }
  ~bad_request_error() override;
};
}
