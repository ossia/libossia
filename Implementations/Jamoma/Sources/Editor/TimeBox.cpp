/*!
 * \file TimeBox.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/TimeBox.h"

#include "TTScore.h"

namespace OSSIA
{
  class TimeBox::Impl
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
  
  TimeBox::TimeBox() :
  pimpl(new Impl)
  {}
  
  TimeBox::TimeBox(const TimeBox & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  TimeBox::~TimeBox()
  {
    delete pimpl;
  }
  
  TimeBox& TimeBox::operator= (const TimeBox & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
}
