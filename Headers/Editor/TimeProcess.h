/*!
 * \file TimeProcess.h
 *
 * \defgroup Editor
 *
 * \brief #TimeProcess is the mother class to describe temporal content ...
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
#include <string>

namespace OSSIA
{
class StateElement;
class TimeConstraint;
class TimeValue;

class TimeProcess
{

public:

# pragma mark -
# pragma mark Life cycle

  /*! destructor */
  virtual ~TimeProcess();

# pragma mark -
# pragma mark Execution

  /*! get a #StateElement of the process for a position or a date
   \param const #TimeValue position
   \param const #TimeValue date
   \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> state(const TimeValue&, const TimeValue&) = 0;

# pragma mark -
# pragma mark Accessors

  /*! get the parent #TimeConstraint
   \return std::shared_ptr<#TimeConstraint> */
  virtual const std::shared_ptr<TimeConstraint> & getParentTimeConstraint() const = 0;
};
}
