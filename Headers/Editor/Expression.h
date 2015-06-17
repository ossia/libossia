/*!
 * \file Expression.h
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

namespace OSSIA
{

class Expression
{

public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~Expression() = default;
  
# pragma mark -
# pragma mark Execution
  
  /*! evaluate the expression 
   \return bool result of the evaluation */
  virtual bool evaluate() const = 0;

};
}

