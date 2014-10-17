/*!
 * \file Scenario.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef SCENARIO_H_
#define SCENARIO_H_

#include <set>

#include "Editor/TimeNode.h"
#include "Editor/TimeProcess.h"

namespace OSSIA {

class TimeBox;
class TimeNode;

class Scenario : public TimeProcess {

public:

  // Constructors, destructor, assignment
  Scenario();
  Scenario(const Scenario&);
  ~Scenario();
  Scenario & operator= (const Scenario&);

  // Lecture
  virtual void play() const override;

  // Navigation
  std::set<TimeBox*> getTimeBoxes() const;
  std::set<TimeNode*> getTimeNodes() const;

  // Edition
  void addTimeBox(const TimeBox&, const TimeNode & startNode);
  void addTimeBox(
      const TimeBox&,
      const TimeNode & startNode,
      const TimeNode & endNode);

  // Accessors
  // internal TimeNodes
  TimeNode & getStartNode() const;
  void setStartNode(const TimeNode&);
  TimeNode & getEndNode() const;
  void setEndNode(const TimeNode&);

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* SCENARIO_H_ */
