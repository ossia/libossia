/*!
 * \file InteractiveEvent.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef INTERACTIVEEVENT_H_
#define INTERACTIVEEVENT_H_

#include "Event.h"

namespace OSSIA {

class InteractiveEvent : public Event {

  // Constructors, destructor, assignment
  InteractiveEvent();

  // Accessors
  float getPreListenningDuration() const;
  void setPreListenningDuration(float);
  float getSimultaneityMargin();
  void setSimultaneityMargin(float);
  bool isMultiple();

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* INTERACTIVEEVENT_H_ */
