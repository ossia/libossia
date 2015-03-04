/*!
 * \file TimeProcessList.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef TIMEPROCESSLIST_H_
#define TIMEPROCESSLIST_H_

#include <memory>

#include "Misc/Container.h"

namespace OSSIA {

class TimeProcess;

//todo could be just a Container<TimeProcess> ? needs observation ? save/load ?
class TimeProcessList : public Container<TimeProcess> {

public:

  virtual ~TimeProcessList() = default;

};

}



#endif /* TIMEPROCESSLIST_H_ */
