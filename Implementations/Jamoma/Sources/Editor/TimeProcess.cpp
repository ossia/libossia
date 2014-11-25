/*!
 * \file TimeProcess.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/TimeProcess.h"

#include "TTScore.h"

namespace OSSIA
{
  class TimeProcess::Impl
  {
    
  public:
    
    Impl()
    {
      // todo : move this else where ...
      TTFoundationInit("/usr/local/jamoma/");
      TTModularInit("/usr/local/jamoma/");
      TTScoreInit("/usr/local/jamoma/");
    };
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
  };
  
  TimeProcess::TimeProcess() :
  pimpl(new Impl)
  {}
  
  TimeProcess::~TimeProcess()
  {
    delete pimpl;
  }
  
  void TimeProcess::play() const
  {
    ;
  }
}