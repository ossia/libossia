/*!
 * \file State_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/State_Impl.h"

#include "TTScore.h"

namespace OSSIA
{
  State_Impl::State_Impl()
  {
    // todo : move this else where ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  State_Impl::~State_Impl()
  {}
  
  void State_Impl::launch() const
  {
    ;
  }
}