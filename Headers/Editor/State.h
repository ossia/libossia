/*!
 * \file State.h
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

#include "Editor/StateElement.h"
#include "Misc/Container.h"
#include <ossia_export.h>

namespace OSSIA
{

class OSSIA_EXPORT State : public virtual StateElement
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \return std::shared_ptr<#State> */
  static std::shared_ptr<State> create();

  /*! clone */
  virtual std::shared_ptr<State> clone() const = 0;

  /*! destructor */
  virtual ~State();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! launch each state elements of the state */
  virtual void launch() const override = 0;

#if 0
# pragma mark -
# pragma mark State Elements
#endif

  /*! get the state elements of the state
   \return #Container<#StateElements> */
  Container<StateElement>& stateElements()
  { return m_stateElements; }

  /*! get the state elements of the state
   \return #Container<#StateElements> */
  const Container<StateElement>& stateElements() const
  { return m_stateElements; }

private:
  Container<StateElement> m_stateElements;
};

}
