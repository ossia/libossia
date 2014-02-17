/*!
 * \file IObserver.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef IOBSERVER_H_
#define IOBSERVER_H_

namespace OSSIA {

class IObserver {

  // Constructors, destructor, assignment
  IObserver();
  virtual ~IObserver();

  // Notification receiver
  virtual void update(int message, IObservable&) = 0;

};

}

#endif /* IOBSERVER_H_ */
