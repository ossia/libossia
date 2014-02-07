/*!
 * \file Score.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef SCORE_H_
#define SCORE_H_

#include <vector>

namespace OSSIA {

class Event;
class TimeProcess;

class Score {

  Score();

  void play() const;

  void pause() const;

  void stop() const;

  void goToSecond(float date) const;

  float getLength() const;

  Event & getStartEvent();
  Event & getEndEvent();

  std::vector<TimeProcess*> getTimeProcesses;

};

}

#endif /* SCORE_H_ */
