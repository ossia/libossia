/*!
 * \file Expression.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

namespace OSSIA {

class Address;

class Expression {

public:

  virtual ~Expression() = default;

  virtual bool evaluate() const = 0;

};

}



#endif /* EXPRESSION_H_ */
