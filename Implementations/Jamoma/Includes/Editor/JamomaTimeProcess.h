/*!
 * \file JamomaTimeProcess.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/Clock.h"
#include "Editor/Message.h"
#include "Editor/State.h"
#include "Editor/TimeProcess.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeProcess : public virtual TimeProcess
{

protected:

# pragma mark -
# pragma mark Implementation specific

  TimeValue                       mLastDate;  // used to filter multiple state calls at the same time (use date as position can be always 0 in infinite duration case)

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeProcess();

# pragma mark -
# pragma mark Execution - Implementation specific

  virtual void start() = 0;
  virtual void stop() = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;

# pragma mark -
# pragma mark Implementation specific

  /*! append each message of the state to the current state in order to eliminate address redundancy 
   \param shared_ptr<State> the State to fill
   \param shared_ptr<StateElement> the StateElement to store */
  void flattenAndFilter(shared_ptr<State>, const shared_ptr<StateElement>&);
};
