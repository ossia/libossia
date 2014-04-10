/*!
 * \file IObservable.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef IOBSERVABLE_H_
#define IOBSERVABLE_H_

namespace OSSIA {

class IObserver;

class IObservable {

public:

  // Constructors, destructor, assignment
  IObservable();
  virtual ~IObservable();

  // Managing Subscribers
  void subscribe(int message, IObserver&);
  void unsubscribe(int message, IObserver&);
  void notify(int message);

};

}

#endif /* IOBSERVABLE_H_ */
