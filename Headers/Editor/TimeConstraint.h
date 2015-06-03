/*!
 * \file TimeConstraint.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
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

  // Constructors, destructor, cloning
  static std::shared_ptr<TimeConstraint> create(TimeValue nominal,
                            TimeValue min,
                            TimeValue max);
  virtual std::shared_ptr<TimeConstraint> clone() const = 0;
  virtual ~TimeConstraint() = default;

  // Execution

  /*! execute the scenario and optionnaly log the execution into a file
   \param bool to enable log
   \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const = 0;

  // Accessors
  
  /*! get the node from where the constraint starts
   \return std::shared_ptr<#TimeNode> start node */
  virtual const std::shared_ptr<TimeNode> & getStartNode() const = 0;
  virtual const std::shared_ptr<TimeNode> & getEndNode() const = 0;
  virtual const std::shared_ptr<State> & getStartState() const = 0;
  virtual void setStartState(std::shared_ptr<State>) = 0;
  virtual const std::shared_ptr<State> & getEndState() const = 0;
  virtual void setEndState(std::shared_ptr<State>) = 0;

  Container<TimeProcess>& timeProcesses()
  { return m_timeProcesses; }
  
  const Container<TimeProcess>& timeProcesses() const
  { return m_timeProcesses; }

  private:
  Container<TimeProcess> m_timeProcesses;
};

}

