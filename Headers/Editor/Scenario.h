/*!
 * \file Scenario.h
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

#include "Editor/TimeProcess.h"

namespace OSSIA
{

class TimeConstraint;
class TimeNode;

class Scenario : public virtual TimeProcess
{

public:

  // Life cycle
  
  /*! factory */
  static std::shared_ptr<Scenario> create();
  
  /*! clone */
  virtual std::shared_ptr<Scenario> clone() const = 0;
  
  /*! destructor */
  virtual ~Scenario() = default;

  // Execution
  
  /*! execute the scenario and optionnaly log the execution into a file
   \param bool to enable log
   \param string to give a log file name where to write */
  virtual void play(bool log = false, std::string name = "") const override = 0;

  // Edition
  
  /*! add a constraint from a start node into the scenario (implicit creation of the end node)
   \param std::shared_ptr<#TimeConstraint> to add
   \param std::shared_ptr<#TimeNode> to start from */
  virtual void addConstraint(const std::shared_ptr<TimeConstraint>, const std::shared_ptr<TimeNode>) = 0;
  
  /*! add a constraint from a start node to an end node into the scenario
   \param std::shared_ptr<#TimeConstraint> to add
   \param std::shared_ptr<#TimeNode> to start from
   \param std::shared_ptr<#TimeNode> to end */
  virtual void addConstraint(const std::shared_ptr<TimeConstraint>, const std::shared_ptr<TimeNode>, const std::shared_ptr<TimeNode>) = 0;
  
  /*! remove a constraint from the scenario
   \param std::shared_ptr<#TimeConstraint> to remove */
  virtual void removeConstraint(const std::shared_ptr<TimeConstraint> constraint) = 0;

  // Accessors
  
  /*! is the scenario allowed to kill sub process when it ends ?
   \return bool as killer status */
  virtual const bool isKiller() const = 0;
  
  /*! allow scenario to kill sub process when it ends
   \param bool as killer status */
  virtual void setKiller(bool) = 0;
  
  /*! get the node from where the scenario starts
   \return std::shared_ptr<#TimeNode> start node */
  virtual const std::shared_ptr<TimeNode> & getStartNode() const = 0;
  
  /*! set the node from where the scenario starts
   \param std::shared_ptr<#TimeNode> start node */
  virtual void setStartNode(std::shared_ptr<TimeNode>) = 0;
  
  /*! get the node where the scenario ends
   \return std::shared_ptr<#TimeNode> end node */
  virtual const std::shared_ptr<TimeNode> & getEndNode() const = 0;
  
  /*! set the node where the scenario ends
   \param std::shared_ptr<#TimeNode> end node */
  virtual void setEndNode(std::shared_ptr<TimeNode>) = 0;
};

}
