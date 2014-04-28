/*!
 * \file branching.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Scenario.h"
#include "State.h"
#include "TimeBox.h"

using namespace OSSIA;

void main() {

  // States

  State stStart;
  State stEnd;
  State stChoice;
  State stA;
  State stB1;
  State stB2;

  // TimeBoxes

  TimeBox A;
  TimeBox B1;
  TimeBox B2;

  // Scenario

  Scenario branching;
  branching.setStartState(stStart);
  branching.setEndState(stEnd);

}
