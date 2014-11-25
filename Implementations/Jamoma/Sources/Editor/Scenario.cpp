/*!
 * \file Scenario.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Scenario.h"

#include "TTScore.h"

namespace OSSIA
{
  class Scenario::Impl
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
  
  
  Scenario::Scenario() :
  pimpl(new Impl)
  {}
  
  Scenario::Scenario(const Scenario & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  Scenario::~Scenario()
  {
    delete pimpl;
  }
  
  Scenario& Scenario::operator= (const Scenario & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
  
  void Scenario::play() const
  {
    ;
  }
}