/*!
 * \file StateElement.h
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

namespace OSSIA
{

class StateElement
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~StateElement() = default;
  
# pragma mark -
# pragma mark Execution
  
  /*! what to do at execution time */
  virtual void launch() const = 0;
};

}
