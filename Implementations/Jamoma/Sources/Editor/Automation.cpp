/*!
 * \file Automation.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Automation.h"

#include "TTScore.h"

namespace OSSIA
{
  class Automation::Impl
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
  
  
  Automation::Automation() :
  pimpl(new Impl)
  {}
  
  Automation::Automation(const Automation & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  Automation::~Automation()
  {
    delete pimpl;
  }
  
  Automation& Automation::operator= (const Automation & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
  
  void Automation::play() const
  {
    ;
  }
}