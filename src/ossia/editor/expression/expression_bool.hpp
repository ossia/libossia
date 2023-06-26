#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/editor/expression/expression_fwd.hpp>

/**
 * \file expression_bool.hpp
 */
namespace ossia::expressions
{
/**
 * @brief expression_bool : a constant boolean value.
 * @see expression.hpp
 */
class OSSIA_EXPORT expression_bool final : public expression_callback_container
{
  bool m_result{};

public:
  expression_bool(bool result);
  expression_bool(expression_bool&& other) = delete;
  expression_bool& operator=(expression_bool&& other) = delete;

  expression_bool(const expression_bool& other) = delete;
  expression_bool& operator=(const expression_bool& other) = delete;

  ~expression_bool();

  bool evaluate() const noexcept { return m_result; }
  void update() const noexcept { }
  void reset() const noexcept { }
};
}
