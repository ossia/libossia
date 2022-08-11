// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/expression/expression.hpp>

namespace ossia::expressions
{
namespace
{
struct evaluate_visitor
{
  template <typename T>
  bool operator()(const T& e)
  {
    return e.evaluate();
  }
  bool operator()(const ossia::monostate& e) { return false; }
};

struct update_visitor
{
  template <typename T>
  void operator()(const T& e)
  {
    e.update();
  }
  void operator()(const ossia::monostate& e) { return; }
};

struct reset_visitor
{
  template <typename T>
  void operator()(T& e)
  {
    e.reset();
  }
  void operator()(ossia::monostate& e) { return; }
};

struct add_callback_visitor
{
  expression_result_callback cb;

  template <typename T>
  expression_callback_iterator operator()(T& e)
  {
    return e.add_callback(std::move(cb));
  }

  expression_callback_iterator operator()(ossia::monostate& e) { return {}; }
};

struct remove_callback_visitor
{
  expression_callback_iterator it;
  template <typename T>
  void operator()(T& e)
  {
    e.remove_callback(it);
  }
  void operator()(ossia::monostate& e) { }
};

struct get_callback_count_visitor
{
  template <typename T>
  std::size_t operator()(const T& e)
  {
    return e.callback_count();
  }
  std::size_t operator()(const ossia::monostate& e) { return 0; }
};

struct different_visitor
{
  template <typename T, typename U>
  bool operator()(const T& lhs, const U& rhs)
  {
    return true;
  }

  bool operator()(const expression_atom& lhs, const expression_atom& rhs)
  {
    return lhs.get_first_operand() != rhs.get_first_operand()
           || lhs.get_operator() != rhs.get_operator()
           || lhs.get_second_operand() != rhs.get_second_operand();
  }

  bool operator()(const expression_bool& lhs, const expression_bool& rhs)
  {
    return lhs.evaluate() != rhs.evaluate();
  }

  bool operator()(const expression_composition& lhs, const expression_composition& rhs)
  {
    return lhs.get_operator() != rhs.get_operator()
           || ossia::apply_nonnull(
               *this, lhs.get_first_operand(), rhs.get_first_operand())
           || ossia::apply_nonnull(
               *this, lhs.get_second_operand(), rhs.get_second_operand());
  }

  bool operator()(const expression_not& lhs, const expression_not& rhs)
  {
    return ossia::apply_nonnull(*this, lhs.get_expression(), rhs.get_expression());
  }

  bool operator()(const expression_pulse& lhs, const expression_pulse& rhs)
  {
    return lhs.get_destination() != rhs.get_destination();
  }
};

struct equal_visitor
{
  template <typename T, typename U>
  bool operator()(const T& lhs, const U& rhs)
  {
    return false;
  }

  bool operator()(const expression_atom& lhs, const expression_atom& rhs)
  {
    return lhs.get_operator() == rhs.get_operator()
           && lhs.get_first_operand() == rhs.get_first_operand()
           && lhs.get_second_operand() == rhs.get_second_operand();
  }

  bool operator()(const expression_bool& lhs, const expression_bool& rhs)
  {
    return lhs.evaluate() == rhs.evaluate();
  }

  bool operator()(const expression_composition& lhs, const expression_composition& rhs)
  {
    return lhs.get_operator() == rhs.get_operator()
           && ossia::apply_nonnull(
               *this, lhs.get_first_operand(), rhs.get_first_operand())
           && ossia::apply_nonnull(
               *this, lhs.get_second_operand(), rhs.get_second_operand());
  }

  bool operator()(const expression_not& lhs, const expression_not& rhs)
  {
    return ossia::apply_nonnull(*this, lhs.get_expression(), rhs.get_expression());
  }

  bool operator()(const expression_pulse& lhs, const expression_pulse& rhs)
  {
    return lhs.get_destination() == rhs.get_destination();
  }
};
}

bool evaluate(const ossia::expressions::expression_base& e)
{
  return ossia::apply_nonnull(evaluate_visitor{}, e);
}

void update(const ossia::expressions::expression_base& e)
{
  return ossia::apply_nonnull(update_visitor{}, e);
}

void reset(ossia::expressions::expression_base& e)
{
  return ossia::apply_nonnull(reset_visitor{}, e);
}

bool operator!=(const expression_base& lhs, const expression_base& rhs)
{
  return ossia::apply_nonnull(different_visitor{}, lhs, rhs);
}

bool operator==(const expression_base& lhs, const expression_base& rhs)
{
  return ossia::apply_nonnull(equal_visitor{}, lhs, rhs);
}

expression_callback_iterator
add_callback(expression_base& e, expression_result_callback cb)
{
  return ossia::apply_nonnull(add_callback_visitor{std::move(cb)}, e);
}

void remove_callback(expression_base& e, expression_callback_iterator it)
{
  return ossia::apply_nonnull(remove_callback_visitor{it}, e);
}

std::size_t callback_count(expression_base& e)
{
  return ossia::apply_nonnull(get_callback_count_visitor{}, e);
}

const expression_base& expression_true()
{
  static const expression_base e{ossia::in_place_type<expression_bool>, true};
  return e;
}

const expression_base& expression_false()
{
  static const expression_base e{ossia::in_place_type<expression_bool>, false};
  return e;
}

expression_ptr make_expression_atom(
    const expression_atom::val_t& lhs, comparator c, const expression_atom::val_t& rhs)
{
  return std::make_unique<expression_base>(
      ossia::in_place_type<expression_atom>, lhs, c, rhs, expression_atom::dummy_t{});
}

expression_ptr make_expression_atom(
    expression_atom::val_t&& lhs, comparator c, expression_atom::val_t&& rhs)
{
  return std::make_unique<expression_base>(
      ossia::in_place_type<expression_atom>, std::move(lhs), c, std::move(rhs),
      expression_atom::dummy_t{});
}
}
