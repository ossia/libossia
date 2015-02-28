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

#include "Misc/Iterators.h"

#include "Editor/StateElement.h"

namespace OSSIA {

class State : public StateElement {

public:

  // Factories, destructor
  static std::shared_ptr<State> create();
  virtual std::shared_ptr<State> clone() const = 0;
  virtual ~State() = default;

  // Lecture
  virtual void launch() const override = 0;

  // Std container
  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual const_iterator cbegin() const = 0;
  virtual const_iterator cend() const = 0;
  virtual size_type size() const = 0;
  virtual bool empty() const = 0;
  virtual std::shared_ptr<StateElement> & front() = 0;
  virtual const std::shared_ptr<StateElement> & front() const = 0;
  virtual std::shared_ptr<StateElement> & back() = 0;
  virtual const std::shared_ptr<StateElement> & back() const = 0;
  virtual iterator insert(const_iterator, std::shared_ptr<StateElement>) = 0;
  virtual iterator erase(const_iterator) = 0;
  virtual iterator erase(const_iterator first, const_iterator last) = 0;

};

}

#endif /* STATE_H_ */
