/*!
 * \file Event.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <vector>

namespace OSSIA {

class Address;
class StateParameter;

class Event {

  Event();

  std::vector<StateParameter> getState() const;

  StateParameter getStateParameter(Address addr) const;

  Event & setStateParameter(StateParameter sp);

  template <typename T>
  Event & setStateParameter(Address addr, T value);

  float getDate() const;

};

}

#endif /* EVENT_H_ */
