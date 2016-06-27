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
#include <ossia_export.h>

namespace OSSIA
{

class OSSIA_EXPORT StateElement
{

public:

#if 0
# pragma mark -
# pragma mark Enumerations
#endif

  /*! type of element */
  enum class Type
  {
    MESSAGE,
    //! \todo ALIAS to refer to another message,
    //! \todo QUERY to ask the value of an address,
    STATE,
    USER
  };

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! destructor */
  virtual ~StateElement();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! what to do at execution time */
  virtual void launch() const = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the type of the state element
   \return #Type of the state element */
  virtual Type getType() const = 0;
};

}
