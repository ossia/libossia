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

#include "Misc/Container.h"

namespace OSSIA
{

class State;
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
   \param #TimeValue duration of the constraint
   \param #TimeValue minimal duration of the constraint
   \param #TimeValue maximal duration of the constraint 
   \return std::shared_ptr<#TimeConstraint> */
  static std::shared_ptr<TimeConstraint> create(TimeValue nominal,
                                                TimeValue min,
                                                TimeValue max);
  
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
  
  /*! get the node from where the constraint starts
   \return std::shared_ptr<#TimeNode> start node */
  virtual const std::shared_ptr<TimeNode> & getStartNode() const = 0;
  
  /*! get the node from where the constraint starts
   \return std::shared_ptr<#TimeNode> start node */
  virtual const std::shared_ptr<TimeNode> & getEndNode() const = 0;
  
  /*! get the state on start
   \return std::shared_ptr<#State> start state */
  virtual const std::shared_ptr<State> & getStartState() const = 0;
  
  /*! set the state on start
   \param std::shared_ptr<#State> start state */
  virtual void setStartState(std::shared_ptr<State>) = 0;
  
  /*! get the state at end
   \return std::shared_ptr<#State> end state */
  virtual const std::shared_ptr<State> & getEndState() const = 0;
  
  /*! set the state at end
   \param std::shared_ptr<#State> end state */
  virtual void setEndState(std::shared_ptr<State>) = 0;

# pragma mark -
# pragma mark Time Processes
  
  /*! get processes attached to the constraint
   \return #Container<#TimeProcess> container */
  Container<TimeProcess>& timeProcesses()
  { return m_timeProcesses; }
  
  /*! get processes attached to the constraint
   \return #Container<#TimeProcess> container */
  const Container<TimeProcess>& timeProcesses() const
  { return m_timeProcesses; }

private:
  Container<TimeProcess> m_timeProcesses;
};

}

