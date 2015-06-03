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

      // Constructors, destructor, cloning
      static std::shared_ptr<ExpressionNot> create(std::shared_ptr<Expression>);
      virtual std::shared_ptr<ExpressionNot> clone() const = 0;
      virtual ~ExpressionNot() = default;

      // Execution
      virtual bool evaluate() const override = 0;

      // Accessors
      virtual const std::shared_ptr<Expression> & getExpression() const = 0;

    };

}
