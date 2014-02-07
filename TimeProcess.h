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

namespace OSSIA {

class Event;

class TimeProcess {

  TimeProcess();

  void play() const;

  Event & getStartEvent() const;
  Event & getEndEvent() const;

  float getLength() const;
//??  void setLength(float length);
  float getMinimumLength() const;
  void setMinimumLength(float minLength);
  float getMaximumLength() const;
  void setMaximumLength(float maxLength);

};

}

#endif /* TIMEPROCESS_H_ */
