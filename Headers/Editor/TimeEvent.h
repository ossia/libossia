/*!
 * \file TimeEvent.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEEVENT_H_
#define TIMEEVENT_H_

#include <memory>
#include <string>

namespace OSSIA {

class TimeEvent {

public:

  virtual ~TimeEvent() = default;

  // Lecture
  void play(bool log = false, std::string name = "") const = 0;

};

}



#endif /* TIMEEVENT_H_ */
