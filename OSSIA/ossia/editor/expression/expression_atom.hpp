/*!
 * \file ExpressionAtom.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>

#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/value/value.hpp>

#include <ossia/network/base/address.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace expressions
{
class OSSIA_EXPORT expression_atom : public expression_base
{
  ossia::value mFirstValue;
  Operator mOperator;
  ossia::value mSecondValue;

  net::address::callback_index mFirstValueCallbackIndex;
  net::address::callback_index mSecondValueCallbackIndex;

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of operator */
  enum class Operator
  {
    EQUAL,
    DIFFERENT,
    GREATER_THAN,
    LOWER_THAN,
    GREATER_THAN_OR_EQUAL,
    LOWER_THAN_OR_EQUAL
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  expression_atom(const value& lhs,
                  Operator op = Operator::EQUAL,
                  const value& rhs = Impulse{}):
    mFirstValue(lhs),
    mOperator(op),
    mSecondValue(rhs)
  {

  }

  /*! destructor */
  virtual ~expression_atom();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! evaluate the expression atom
   \return bool result of the evaluation */
  virtual bool evaluate() const override
  {
    return do_evaluation(mFirstValue, mSecondValue);
  }

  /*! pull the value of any #Destination operand */
  virtual void update() const override
  {
    // pull value of the first operand if it is a Destination
    if (mFirstValue.getType() == ossia::Type::DESTINATION)
    {
      auto& d = mFirstValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        addr->pullValue();
      }
    }

    // pull value of the second operand if it is a Destination
    if (mSecondValue.getType() == ossia::Type::DESTINATION)
    {
      auto& d = mSecondValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        addr->pullValue();
      }
    }
  }

  bool operator!= (const expression_base& exp) const
  {
    if (exp.getType() == expression_base::Type::ATOM)
    {
      const JamomaExpressionAtom e = dynamic_cast<const JamomaExpressionAtom&>(exp);
      return value{mFirstValue} != value{e.mFirstValue} ||
    mOperator != e.mOperator ||
                 value{mSecondValue} != value{e.mSecondValue};
    }
    else
      return true;
  }


  bool operator== (const expression_base& exp) const
  {
    if (exp.getType() == expression_base::Type::ATOM)
    {
      const JamomaExpressionAtom e = dynamic_cast<const JamomaExpressionAtom&>(exp);
      return value{mFirstValue} == value{e.mFirstValue} &&
              mOperator == e.mOperator &&
               value{mSecondValue} == value{e.mSecondValue};
    }
    else
      return false;
  }



#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the type of the expression
   \return #Type of the expression */
  expression_base::Type getType() const override final
  {return expression_base::Type::ATOM;}

  /*! get first operand
   \return const #Value* first operand */
  const value& getFirstOperand() const
  {
    return mFirstValue;
  }

  /*! get operator
   \return #Operator operator */
  Operator getOperator() const
  {
    return mOperator;
  }

  /*! get second operand
   \return const #Value* second operand */
  virtual const value& getSecondOperand() const
  {
    return mSecondValue;
  }

  void onFirstCallbackAdded()
  {
    // start first operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mFirstValue.getType() == ossia::Type::DESTINATION)
    {
      auto& d = mFirstValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        mFirstValueCallbackIndex = addr->addCallback(
                                     [&] (const ossia::value& result) { firstValueCallback(result); });
      }
    }

    // start second operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mSecondValue.getType() == ossia::Type::DESTINATION)
    {
      auto& d = mSecondValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        mSecondValueCallbackIndex = addr->addCallback(
                                      [&] (const ossia::value& result) { secondValueCallback(result); });
      }
    }
  }

  void onRemovingLastCallback()
  {
    // stop first operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mFirstValue.getType() == ossia::Type::DESTINATION)
    {
      auto& d = mFirstValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        addr->removeCallback(mFirstValueCallbackIndex);
      }
    }

    // start second operand observation if it is a Destination
    //! \todo what about Tuple of Destinations ?
    if (mSecondValue.getType() == ossia::Type::DESTINATION)
    {
      auto& d = mSecondValue.get<Destination>();
      if (const auto& addr = d.value->getAddress())
      {
        addr->removeCallback(mSecondValueCallbackIndex);
      }
    }
  }

  bool do_evaluation(const value& first, const value& second) const
  {
    switch (mOperator)
    {
      case Operator::EQUAL :
      {
        return first == second;
      }
      case Operator::DIFFERENT :
      {
        return first != second;
      }
      case Operator::GREATER_THAN :
      {
        return first > second;
      }
      case Operator::LOWER_THAN :
      {
        return first < second;
      }
      case Operator::GREATER_THAN_OR_EQUAL :
      {
        return first >= second;
      }
      case Operator::LOWER_THAN_OR_EQUAL :
      {
        return first <= second;
      }
      default :
        return false;
    }
  }

  void firstValueCallback(const ossia::value& value)
  {
    if(mSecondValue.valid())
      send(do_evaluation(value, mSecondValue));
  }

  void secondValueCallback(const ossia::value& value)
  {
    if(mSecondValue.valid())
      send(do_evaluation(mFirstValue, value));
  }
};

}
}
