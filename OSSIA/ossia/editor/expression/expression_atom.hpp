#pragma once

#include <memory>

#include <ossia/editor/expression/expression_fwd.hpp>
#include <ossia/editor/expression/operators.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/base/address.hpp>
#include <eggs/variant.hpp>
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
  using val_t = eggs::variant<ossia::value, ossia::Destination>;
  expression_atom(
      const value& lhs, comparator op = comparator::EQUAL,
      const value& rhs = impulse{});
  expression_atom(
      const Destination& lhs, comparator op = comparator::EQUAL,
      const value& rhs = impulse{});
  expression_atom(
      const Destination& lhs, comparator op,
      const Destination& rhs);
  expression_atom(
      const value& lhs, comparator op,
      const Destination& rhs);

  struct dummy_t { };
  expression_atom(
      const val_t& lhs, comparator op,
      const val_t& rhs, dummy_t);

  expression_atom(expression_atom&& other) = delete;
  expression_atom& operator=(expression_atom&& other) = delete;

  expression_atom(const expression_atom& other) = delete;
  expression_atom& operator=(const expression_atom& other) = delete;

  virtual ~expression_atom();

  bool evaluate() const;
  void update() const;

  const val_t& get_first_operand() const;
  comparator get_operator() const;
  const val_t& get_second_operand() const;


  bool operator()(const ossia::value& first, const ossia::value& second) const;
  bool operator()(const ossia::value& first, const ossia::Destination& second) const;
  bool operator()(const ossia::Destination& first, const ossia::value& second) const;
  bool operator()(const ossia::Destination& first, const ossia::Destination& second) const;

private:
  bool operator()(const ossia::value& first, const val_t& second) const;
  bool operator()(const val_t& first, const ossia::value& second) const;
  void on_first_callback_added() override;
  void on_removing_last_callback() override;
  void firstValueCallback(const ossia::value& value);
  void secondValueCallback(const ossia::value& value);

  val_t m_first;
  val_t m_second;

  net::address_base::callback_index m_firstCallback;
  net::address_base::callback_index m_secondCallback;

  comparator m_operator{};
};
}
}
