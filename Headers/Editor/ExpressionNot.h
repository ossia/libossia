/*!
 * \file ExpressionNot.h
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
#include "Editor/Expression.h"

namespace OSSIA {

class ExpressionNot : public Expression {

    public:

    # pragma mark -
# pragma mark Life cycle
      static std::shared_ptr<ExpressionNot> create(std::shared_ptr<Expression>);
      virtual std::shared_ptr<ExpressionNot> clone() const = 0;
      virtual ~ExpressionNot() = default;

    # pragma mark -
# pragma mark Execution
      virtual bool evaluate() const override = 0;

    # pragma mark -
# pragma mark Accessors
      virtual const std::shared_ptr<Expression> & getExpression() const = 0;

    };

}
