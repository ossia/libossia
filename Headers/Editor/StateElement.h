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
# pragma mark Enumerations
  
  /*! type of element */
  enum class Type
  {
    MESSAGE,
    //! \todo ALIAS to refer to another message,
    //! \todo QUERY to ask the value of an address,
    STATE
  };

# pragma mark -
# pragma mark Life cycle
  
  /*! destructor 
   \todo remove = default */
  virtual ~StateElement() = default;
  
# pragma mark -
# pragma mark Execution
  
  /*! what to do at execution time */
  virtual void launch() const = 0;
  
# pragma mark -
# pragma mark Accessors
  
  /*! get the type of the state element
   \return #Type of the state element */
  virtual Type getType() const = 0;
};

}
