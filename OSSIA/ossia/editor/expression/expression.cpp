#include <ossia/editor/expression/expression.hpp>

namespace ossia
{
namespace expressions
{
namespace
{
struct evaluate_visitor
{
  template<typename T>
  bool operator()(const T& e)
  {
    return e.evaluate();
  }
};

struct update_visitor
{
  template<typename T>
  void operator()(const T& e)
  {
    e.update();
  }
};

struct add_callback_visitor
{
  expression_result_callback cb;

  template<typename T>
  expression_callback_iterator operator()(T& e)
  {
    return e.addCallback(std::move(cb));
  }
};


struct remove_callback_visitor
{
  expression_callback_iterator it;
  template<typename T>
  void operator()(T& e)
  {
    e.removeCallback(it);
  }
};

struct different_visitor
{
  template<typename T, typename U>
  bool operator()(const T& lhs, const U& rhs)
  {
    return true;
  }

  bool operator()(const expression_atom& lhs, const expression_atom& rhs)
  {
    return
        value(lhs.getFirstOperand()) != value(rhs.getFirstOperand())
        || lhs.getOperator() != rhs.getOperator()
        || value(lhs.getSecondOperand()) != value(rhs.getSecondOperand());
  }

  bool operator()(const expression_bool& lhs, const expression_bool& rhs)
  {
    return lhs.evaluate() != rhs.evaluate();
  }

  bool operator()(const expression_composition& lhs, const expression_composition& rhs)
  {
    return
        lhs.getOperator() != rhs.getOperator()
        || eggs::variants::apply(*this, lhs.getFirstOperand(), rhs.getFirstOperand())
        || eggs::variants::apply(*this, lhs.getSecondOperand(), rhs.getSecondOperand());
  }

  bool operator()(const expression_not& lhs, const expression_not& rhs)
  {
    return eggs::variants::apply(*this, lhs.getExpression(), rhs.getExpression());
  }

  bool operator()(const expression_pulse& lhs, const expression_pulse& rhs)
  {
    return ossia::value(lhs.getDestination()) != ossia::value(rhs.getDestination());
  }
};

struct equal_visitor
{
  template<typename T, typename U>
  bool operator()(const T& lhs, const U& rhs)
  {
    return false;
  }

  bool operator()(const expression_atom& lhs, const expression_atom& rhs)
  {
    return
        lhs.getOperator() == rhs.getOperator()
        && value(lhs.getFirstOperand()) == value(rhs.getFirstOperand())
        && value(lhs.getSecondOperand()) == value(rhs.getSecondOperand());
  }

  bool operator()(const expression_bool& lhs, const expression_bool& rhs)
  {
    return lhs.evaluate() == rhs.evaluate();
  }

  bool operator()(const expression_composition& lhs, const expression_composition& rhs)
  {
    return
        lhs.getOperator() == rhs.getOperator()
        && eggs::variants::apply(*this, lhs.getFirstOperand(), rhs.getFirstOperand())
        && eggs::variants::apply(*this, lhs.getSecondOperand(), rhs.getSecondOperand());
  }

  bool operator()(const expression_not& lhs, const expression_not& rhs)
  {
    return eggs::variants::apply(*this, lhs.getExpression(), rhs.getExpression());
  }

  bool operator()(const expression_pulse& lhs, const expression_pulse& rhs)
  {
    return ossia::value(lhs.getDestination()) == ossia::value(rhs.getDestination());
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

expression_callback_iterator addCallback(
    expression_base& e,
    expression_result_callback cb)
{
  return eggs::variants::apply(add_callback_visitor{std::move(cb)}, e);
}

void removeCallback(
    expression_base& e,
    expression_callback_iterator it)
{
  return eggs::variants::apply(remove_callback_visitor{it}, e);
}

}
}
