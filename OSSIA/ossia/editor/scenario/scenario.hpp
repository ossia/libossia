/*!
 * \file Scenario.h
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

#include "Editor/Clock.hpp"
#include "Editor/State.hpp"
#include "Editor/TimeProcess.hpp"
#include <ossia_export.h>

namespace OSSIA
{

class TimeConstraint;
class TimeNode;

class OSSIA_EXPORT Scenario : public virtual TimeProcess
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \return std::shared_ptr<#Scenario> */
  static std::shared_ptr<Scenario> create();

  /*! clone */
  virtual std::shared_ptr<Scenario> clone() const = 0;

  /*! destructor */
  virtual ~Scenario();

#if 0
# pragma mark -
# pragma mark Edition
#endif

  /*! add a #TimeConstraint and its #TimeNodes into the scenario if they don't already be added
   \param std::shared_ptr<#TimeConstraint> to add */
  virtual void addTimeConstraint(std::shared_ptr<TimeConstraint>) = 0;

  /*! remove a #TimeConstraint from the scenario without removing any #TimeNode
   \param std::shared_ptr<#TimeConstraint> to remove */
  virtual void removeTimeConstraint(const std::shared_ptr<TimeConstraint>&) = 0;

  /*! add a #TimeNode into the scenario if it is not already added
   \param std::shared_ptr<#TimeNode> to add */
  virtual void addTimeNode(std::shared_ptr<TimeNode>) = 0;

  /*! remove a #TimeNode from the scenario
   \param std::shared_ptr<#TimeNode> to remove */
  virtual void removeTimeNode(const std::shared_ptr<TimeNode>&) = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the node from where the scenario starts
   \return std::shared_ptr<#TimeNode> start node */
  virtual const std::shared_ptr<TimeNode> & getStartTimeNode() const = 0;

#if 0
# pragma mark -
# pragma mark TimeNodes and TimeConstraints
#endif

  /*! get all TimeNodes of the scenario
   \return #Container<#TimeNode> */
  virtual const Container<TimeNode>& timeNodes() const = 0;

  /*! get all TimeConstraints of the scenario
   \return #Container<#TimeConstraint> */
  virtual const Container<TimeConstraint>& timeConstraints() const = 0;
};

}
