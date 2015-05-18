/*!
 * \file TimeProcessList.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>

#include "Misc/Container.h"

namespace OSSIA {

class TimeProcess;

//TODO could be just a Container<TimeProcess> ? needs observation ? save/load ?
class TimeProcessList : public virtual Container<TimeProcess> {

public:

  virtual ~TimeProcessList() = default;

};

}
