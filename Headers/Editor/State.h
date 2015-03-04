/*!
 * \file State.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef STATE_H_
#define STATE_H_

#include <memory>

#include "Editor/StateElement.h"
#include "Misc/Container.h"

namespace OSSIA {

class State : public StateElement, public Container<StateElement> {

public:

  // Factories, destructor
  static std::shared_ptr<State> create();
  virtual std::shared_ptr<State> clone() const = 0;
  virtual ~State() = default;

  // Lecture
  virtual void launch() const override = 0;

};

}

#endif /* STATE_H_ */
