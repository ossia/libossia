#pragma once

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/expression/operators.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>

#include <ossia/detail/nullable_variant.hpp>
#include <ossia/detail/config.hpp>

#include <memory>

/**
 * \file expression_atom.hpp
 */
namespace ossia
{
namespace expressions
{
/**
 * @brief expression_atom : numeric operators in expressions.
 * @see expression.hpp
 */
class OSSIA_EXPORT expression_atom final : public expression_callback_container
{
public:
  using val_t = ossia::nullable_variant<ossia::value, ossia::destination>;
  expression_atom(
      const value& lhs, comparator op = comparator::EQUAL,
      const value& rhs = impulse{});
  expression_atom(
      const destination& lhs, comparator op = comparator::EQUAL,
      const value& rhs = impulse{});
  expression_atom(
      const destination& lhs, comparator op, const destination& rhs);
  expression_atom(const value& lhs, comparator op, const destination& rhs);

  struct dummy_t
  {
  };
  expression_atom(const val_t& lhs, comparator op, const val_t& rhs, dummy_t);

  expression_atom(expression_atom&& other) = delete;
  expression_atom& operator=(expression_atom&& other) = delete;

  expression_atom(const expression_atom& other) = delete;
  expression_atom& operator=(const expression_atom& other) = delete;

  virtual ~expression_atom();

  bool evaluate() const;
  void update() const;
  void reset();

  const val_t& get_first_operand() const;
  comparator get_operator() const;
  const val_t& get_second_operand() const;

  bool operator()(const ossia::value& first, const ossia::value& second) const;
  bool operator()(
      const ossia::value& first, const ossia::destination& second) const;
  bool operator()(
      const ossia::destination& first, const ossia::value& second) const;
  bool operator()(
      const ossia::destination& first, const ossia::destination& second) const;

  constexpr inline bool operator()(const ossia::value& v, const ossia::monostate& m) const noexcept { return false; }
  constexpr inline bool operator()(const ossia::destination& v, const ossia::monostate& m) const noexcept { return false; }
  constexpr inline bool operator()(const ossia::monostate& v, const ossia::value& m) const noexcept { return false; }
  constexpr inline bool operator()(const ossia::monostate& v, const ossia::destination& m) const noexcept { return false; }
  constexpr inline bool operator()(const ossia::monostate& v, const ossia::monostate& m) const noexcept { return false; }
private:
  bool operator()(const ossia::value& first, const val_t& second) const;
  bool operator()(const val_t& first, const ossia::value& second) const;
  void on_first_callback_added() override;
  void on_removing_last_callback() override;
  void first_value_callback(const ossia::value& value);
  void second_value_callback(const ossia::value& value);

  val_t m_first;
  val_t m_second;

  net::parameter_base::callback_index m_firstCallback;
  net::parameter_base::callback_index m_secondCallback;

  comparator m_operator{};
};
}
}
