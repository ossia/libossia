#pragma once

#include <memory>

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/expression/operators.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/base/address.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
/**
 * @brief expression_atom : numeric operators in expressions.
 * @see expression.hpp
 */
class OSSIA_EXPORT expression_atom final :
    public expression_callback_container
{
public:
  expression_atom(
      const value& lhs, comparator op = comparator::EQUAL,
      const value& rhs = impulse{});
  expression_atom(expression_atom&& other) = delete;
  expression_atom& operator=(expression_atom&& other) = delete;

  expression_atom(const expression_atom& other) = delete;
  expression_atom& operator=(const expression_atom& other) = delete;

  virtual ~expression_atom();

  bool evaluate() const;
  void update() const;

  const value& get_first_operand() const;
  comparator get_operator() const;
  const value& get_second_operand() const;

private:
  void on_first_callback_added() override;
  void on_removing_last_callback() override;

  bool do_evaluation(const value& first, const value& second) const;

  void firstValueCallback(const ossia::value& value);
  void secondValueCallback(const ossia::value& value);

  ossia::value m_first;
  ossia::value m_second;

  net::address_base::callback_index m_firstCallback;
  net::address_base::callback_index m_secondCallback;

  comparator m_operator{};
};
}
}
