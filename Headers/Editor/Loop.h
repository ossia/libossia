/*!
 * \file Loop.h
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

#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeProcess.h"
#include "Misc/Container.h"

namespace OSSIA
{

class TimeValue;

class Loop : public virtual TimeProcess
{
  
public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \return a new loop */
  static std::shared_ptr<Loop> create();
  
  /*! clone */
  virtual std::shared_ptr<Loop> clone() const = 0;
  
  /*! destructor */
  virtual ~Loop() = default;
  
# pragma mark -
# pragma mark Execution
  
    
  /*! get a #State as a flatten set of Messages with no Address redundancy for a position or a date
  \param const #TimeValue position
  \param const #TimeValue date
  \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> state(const TimeValue&, const TimeValue&) = 0;
  
# pragma mark -
# pragma mark Accessors
  
  /*! get the pattern #TimeConstraint
   \return std::shared_ptr<TimeConstraint> */
  virtual const std::shared_ptr<TimeConstraint> getPatternTimeConstraint() const = 0;
  
  /*! get the pattern #TimeNode
   \return std::shared_ptr<TimeNode> */
  virtual const std::shared_ptr<TimeNode> getPatternTimeNode() const = 0;

};
}
