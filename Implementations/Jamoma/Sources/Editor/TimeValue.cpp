/*!
 * \file TimeValue.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/TimeValue.h"

#include "TTScore.h"

namespace OSSIA
{
  class TimeValue::Impl
  {
    
  public:
    
    Impl()
    {
      // todo : move this else where ...
      TTFoundationInit("/usr/local/jamoma/");
      TTModularInit("/usr/local/jamoma/");
      TTScoreInit("/usr/local/jamoma/");
    };
    
    Impl(const int other);
    ~Impl() = default;
  };
  
  TimeValue::TimeValue() :
  pimpl(new Impl)
  {}
  
  TimeValue::TimeValue(const int other) :
  pimpl(new Impl(other))
  {}
  
  TimeValue::~TimeValue()
  {
    delete pimpl;
  }
  
  TimeValue& TimeValue::operator= (const int other)
  {
    delete pimpl;
    pimpl = new Impl(other);
    return *this;
  }
}