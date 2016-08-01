#pragma once
#include <stdexcept>
#include <ossia_export.h>
namespace ossia
{

/**
 * @brief The execution_error class
 *
 * Means that there was a problem
 * during the execution of a score
 */
struct OSSIA_EXPORT execution_error :
    public std::runtime_error
{
  execution_error(const char* e):
    std::runtime_error(e)
  {

  }
};

/**
 * @brief The invalid_value_type_error class
 *
 * Means that an invalid value was passed.
 * For instance a Tuple where an Impulse is required.
 */
struct OSSIA_EXPORT invalid_value_type_error :
    public std::logic_error
{
  invalid_value_type_error(const char* e):
    std::logic_error(e)
  {

  }
};

}
