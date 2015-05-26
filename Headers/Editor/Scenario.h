/*!
 * \file Scenario.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/TimeProcess.h"

namespace OSSIA {

class TimeConstraint;
class TimeNode;

class Scenario : public virtual TimeProcess {

public:

  // Factories, destructor
  static std::shared_ptr<Scenario> create();
  virtual std::shared_ptr<Scenario> clone() const = 0;
  virtual ~Scenario() = default;

  // Lecture
  virtual void play(bool log = false, std::string name = "") const override = 0;

  // Edition
  virtual void addConstraint(const TimeConstraint&, const TimeNode & startNode) = 0;
  virtual void addConstraint(
      const TimeConstraint&,
      const TimeNode & startNode,
      const TimeNode & endNode) = 0;

  // Accessors
  virtual const bool isKiller() const = 0;
  virtual void setKiller(bool) = 0;
  // internal TimeNodes
  virtual const std::shared_ptr<TimeNode> & getStartNode() const = 0;
  virtual void setStartNode(std::shared_ptr<TimeNode>) = 0;
  virtual const std::shared_ptr<TimeNode> & getEndNode() const = 0;
  virtual void setEndNode(std::shared_ptr<TimeNode>) = 0;

};

}
