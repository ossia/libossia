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

#include "IObservable.h"

namespace OSSIA {

class TimeBox;
class TimeValue;

class Score : public IObservable {

  // Constructors, destructor, assignment
  Score();
  Score(const Score&);
  Score(const TimeBox&);
  ~Score();
  Score & operator= (const Score&);

  // Lecture
  TimeValue play() const;
  TimeValue pause() const;
  TimeValue stop() const;

  // Accessors
  TimeBox & getTimeBox() const;
  bool isPlaying() const;
  TimeValue getCursorPosition() const;
  void setCursorPosition(TimeValue) const;

};

}

#endif /* SCORE_H_ */
