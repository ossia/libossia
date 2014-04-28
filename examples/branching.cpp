/*!
 * \file branching.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Event.h"
#include "Expression.h"
#include "InteractiveBranching.h"
#include "Scenario.h"
#include "State.h"
#include "TimeBox.h"

using namespace OSSIA;

void main() {

  // States

  State stStart;
  State stEnd;
  State stA;
  State stB1;
  State stB2;

  // TimeBoxes

  TimeBox A;
  TimeBox B1;
  TimeBox B2;

  // Scenario

  Scenario branching;

  // Setting states

  branching.setStartState(stStart);
  branching.setEndState(stEnd);
  A.getEndEvent().setState(stA);
  B1.getEndEvent().setState(stB1);
  B2.getEndEvent().setState(stB2);

  // Add boxes to Scenario

  branching.addTimeBox(A, branching.getStartEvent());

  // Branching

  InteractiveBranching cond;
  cond.setEvent(A.getEndEvent());
  cond.addBranch(B1.getStartEvent(), "choice == 1");
  cond.addBranch(B2.getStartEvent(), "choice == 2");

  branching.addTimeBox(B1, B1.getStartEvent()); // ??
  branching.addTimeBox(B2, B2.getStartEvent()); // ??

}
