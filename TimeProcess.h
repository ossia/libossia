/*!
 * \file TimeProcess.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEPROCESS_H_
#define TIMEPROCESS_H_

#include "IObservable.h"

namespace OSSIA {

class TimeBox;

class TimeProcess : public IObservable {

public:

  // Constructors, destructor, assignment
  TimeProcess();
  virtual ~TimeProcess();

  // Navigation
  TimeBox & getParentTimeBox();

  // Lecture
  virtual void play() const;

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* TIMEPROCESS_H_ */
