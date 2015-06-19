/*!
 * \file TimeConstraint.h
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
#include <string>

#include "Editor/TimeValue.h"
#include "Misc/Container.h"

namespace OSSIA
{

class TimeNode;
class TimeProcess;
class TimeValue;

class TimeConstraint
{
  
public:
  
# pragma mark -
# pragma mark Life cycle

  /*! factory
   \details if the 3 duration are equals it means ...
   \param #std::shared_ptr<TimeNode> node where the constraint starts
   \param #std::shared_ptr<TimeNode> node where the constraint ends
   \param const #TimeValue& duration of the constraint
   \param const #TimeValue& minimal duration of the constraint
   \param const #TimeValue& maximal duration of the constraint
   \return std::shared_ptr<#TimeConstraint> */
  static std::shared_ptr<TimeConstraint> create(std::shared_ptr<TimeNode>,
                                                std::shared_ptr<TimeNode>,
                                                const TimeValue& = Infinite,
                                                const TimeValue& = 0.,
                                                const TimeValue& = Infinite);
  
  /*! clone 
   \return std::shared_ptr<#TimeConstraint> */
  virtual std::shared_ptr<TimeConstraint> clone() const = 0;
  
  /*! desctructor */
  virtual ~TimeConstraint() = default;

# pragma mark -
# pragma mark Execution

  /*! execute the scenario and optionnaly log the execution into a file
   \param bool to enable log
   \param std::string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get the time constraint duration
   \return const #TimeValue& duration */
  virtual const TimeValue & getDuration() const = 0;
  
  /*! get the time constraint minimal duration
   \return const #TimeValue& minimal duration */
  virtual const TimeValue & getDurationMin() const = 0;
  
  /*! get the time constraint maximal duration
   \return const #TimeValue& maximal duration */
  virtual const TimeValue & getDurationMax() const = 0;
  
  /*! get the node from where the constraint starts
   \return std::shared_ptr<#TimeNode> start node */
  virtual const std::shared_ptr<TimeNode> & getStartNode() const = 0;
  
  /*! get the node from where the constraint ends
   \return std::shared_ptr<#TimeNode> end node */
  virtual const std::shared_ptr<TimeNode> & getEndNode() const = 0;

# pragma mark -
# pragma mark Time Processes
  
  /*! get time processes attached to the time constraint
   \return #Container<#TimeProcess> container */
  Container<TimeProcess>& timeProcesses()
  { return m_timeProcesses; }
  
  /*! get time processes attached to the time constraint
   \return #Container<#TimeProcess> container */
  const Container<TimeProcess>& timeProcesses() const
  { return m_timeProcesses; }

private:
  Container<TimeProcess> m_timeProcesses;
};

}

