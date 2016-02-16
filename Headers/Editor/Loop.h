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
   \param const #TimeValue& duration of the pattern #TimeConstraint
   \param #TimeConstraint::ExecutionCallback to be notified at each step of the loop
   \param #TimeEvent::ExecutionCallback to get start pattern #TimeEvent's status back
   \param #TimeEvent::ExecutionCallback to get end pattern #TimeEvent's status back
   \return a new loop */
  static std::shared_ptr<Loop> create(const TimeValue&,
                                      TimeConstraint::ExecutionCallback,
                                      TimeEvent::ExecutionCallback,
                                      TimeEvent::ExecutionCallback);

  /*! clone */
  virtual std::shared_ptr<Loop> clone() const = 0;

  /*! destructor */
  virtual ~Loop();

# pragma mark -
# pragma mark Execution

  /*! get a #State depending on the parent #TimeConstraint offset
   \details the #State is a flatten set of Messages with no Address redundancy
   \details don't call offset when the parent #TimeConstraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> offset(const TimeValue&) override = 0;

  /*! get a #State depending on the parent #TimeConstraint date
  \details the #State is a flatten set of Messages with no Address redundancy
   \details don't call state when the parent #TimeConstraint is not running
  \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> state() override = 0;

# pragma mark -
# pragma mark Accessors

  /*! get the pattern #TimeConstraint
   \return std::shared_ptr<TimeConstraint> */
  virtual const std::shared_ptr<TimeConstraint> getPatternTimeConstraint() const = 0;

  /*! get the pattern start #TimeNode
   \return std::shared_ptr<TimeNode> */
  virtual const std::shared_ptr<TimeNode> getPatternStartTimeNode() const = 0;

  /*! get the pattern end #TimeNode
   \return std::shared_ptr<TimeNode> */
  virtual const std::shared_ptr<TimeNode> getPatternEndTimeNode() const = 0;

};
}
