/*!
 * \file ExpressionAtom.h
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

#include "Editor/Expression.h"
#include "Editor/Value.h"

#include "Network/Address.h"

namespace OSSIA
{

class ExpressionAtom : public Expression
{

public:

# pragma mark -
# pragma mark Enumerations
  
  /*! type of operator */
  enum class Operator
  {
    EQUAL,
    GREATER_THAN,
    LOWER_THAN,
    GREATER_THAN_OR_EQUAL,
    LOWER_THAN_OR_EQUAL
  };

# pragma mark -
# pragma mark Life cycle
  
  /*! factory for logical condition `
   \return std::shared_ptr<#ExpressionAtom> */
  static std::shared_ptr<ExpressionAtom> create(Value*,
                                                Operator,
                                                Value*);
         
  /*! factory for event condition
   \return std::shared_ptr<#ExpressionAtom> */
  static std::shared_ptr<ExpressionAtom> create(std::shared_ptr<Address>);
  
  /*! clone
   \return std::shared_ptr<#ExpressionAtom> */
  virtual std::shared_ptr<ExpressionAtom> clone() const = 0;
  
  /*! destructor */
  virtual ~ExpressionAtom() = default;

# pragma mark -
# pragma mark Execution
  
  virtual bool evaluate() const override = 0;

# pragma mark -
# pragma mark Accessors
  
  virtual const Value* getFirstOperand() const = 0;
  
  virtual Operator getOperator() const = 0;
  
  virtual const Value* getSecondOperand() const = 0;
};

}
