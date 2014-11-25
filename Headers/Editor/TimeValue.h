/*!
 * \file TimeValue.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEVALUE_H_
#define TIMEVALUE_H_

namespace OSSIA {

class TimeValue {

  // Constructors, destructor, assignment
  TimeValue();
  TimeValue(const int);
  ~TimeValue();
  TimeValue & operator= (const int);

  // Cast
  operator int();
  
  // pimpl idiom
  // private: // todo issue #1
  class Impl;
  Impl * pimpl;

};

}

#endif /* TIMEVALUE_H_ */
