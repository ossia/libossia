#include <ossia/editor/expression/expression.hpp>

namespace ossia
{
namespace expressions
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
};

struct update_visitor
{
  template <typename T>
  void operator()(const T& e)
  {
    e.update();
  }
};

struct add_callback_visitor
{
  expression_result_callback cb;

  template <typename T>
  expression_callback_iterator operator()(T& e)
  {
    return e.add_callback(std::move(cb));
  }
};

struct remove_callback_visitor
{
  expression_callback_iterator it;
  template <typename T>
  void operator()(T& e)
  {
    e.remove_callback(it);
  }
};

struct get_callback_count_visitor
{
  template <typename T>
  std::size_t operator()(const T& e)
  {
    return e.callback_count();
  }
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

  bool operator()(
      const expression_composition& lhs, const expression_composition& rhs)
  {
    return lhs.get_operator() != rhs.get_operator()
           || eggs::variants::apply(
                  *this, lhs.get_first_operand(), rhs.get_first_operand())
           || eggs::variants::apply(
                  *this, lhs.get_second_operand(), rhs.get_second_operand());
  }

  bool operator()(const expression_not& lhs, const expression_not& rhs)
  {
    return eggs::variants::apply(
        *this, lhs.get_expression(), rhs.get_expression());
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

  bool operator()(
      const expression_composition& lhs, const expression_composition& rhs)
  {
    return lhs.get_operator() == rhs.get_operator()
           && eggs::variants::apply(
                  *this, lhs.get_first_operand(), rhs.get_first_operand())
           && eggs::variants::apply(
                  *this, lhs.get_second_operand(), rhs.get_second_operand());
  }

  bool operator()(const expression_not& lhs, const expression_not& rhs)
  {
    return eggs::variants::apply(
        *this, lhs.get_expression(), rhs.get_expression());
  }

  bool operator()(const expression_pulse& lhs, const expression_pulse& rhs)
  {
    return lhs.get_destination() == rhs.get_destination();
  }
};
}

bool evaluate(const ossia::expressions::expression_base& e)
{
  return eggs::variants::apply(evaluate_visitor{}, e);
}

void update(const ossia::expressions::expression_base& e)
{
  return eggs::variants::apply(update_visitor{}, e);
}

bool operator!=(const expression_base& lhs, const expression_base& rhs)
{
  return eggs::variants::apply(different_visitor{}, lhs, rhs);
}

bool operator==(const expression_base& lhs, const expression_base& rhs)
{
  return eggs::variants::apply(equal_visitor{}, lhs, rhs);
}

expression_callback_iterator
add_callback(expression_base& e, expression_result_callback cb)
{
  return eggs::variants::apply(add_callback_visitor{std::move(cb)}, e);
}

void remove_callback(expression_base& e, expression_callback_iterator it)
{
  return eggs::variants::apply(remove_callback_visitor{it}, e);
}

std::size_t callback_count(expression_base& e)
{
  return eggs::variants::apply(get_callback_count_visitor{}, e);
}

const expression_base& expression_true()
{
  static const expression_base e{eggs::variants::in_place<expression_bool>,
                                 true};
  return e;
}

const expression_base& expression_false()
{
  static const expression_base e{eggs::variants::in_place<expression_bool>,
                                 false};
  return e;
}

expression_ptr make_expression_atom(
    const expression_atom::val_t& lhs, comparator c,
    const expression_atom::val_t& rhs)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_atom>, lhs, c, rhs,
      expression_atom::dummy_t{});
}

expression_ptr make_expression_atom(
    expression_atom::val_t&& lhs, comparator c, expression_atom::val_t&& rhs)
{
  return std::make_unique<expression_base>(
      eggs::variants::in_place<expression_atom>, std::move(lhs), c,
      std::move(rhs), expression_atom::dummy_t{});
}
}
}
