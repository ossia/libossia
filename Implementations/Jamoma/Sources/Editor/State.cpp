/*!
 * \file State.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/State.h"

#include "TTScore.h"

namespace OSSIA
{
  class State::Impl
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
  
  State::State() :
  pimpl(new Impl)
  {}
  
  State::State(const State & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  State::~State()
  {
    delete pimpl;
  }
  
  State& State::operator= (const State & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
}