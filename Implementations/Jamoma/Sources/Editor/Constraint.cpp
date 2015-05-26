/*!
 * \file Constraint.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Constraint.h"

#include "TTScore.h"

namespace OSSIA
{
  class Constraint::Impl
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
  
  Constraint::Constraint() :
  pimpl(new Impl)
  {}
  
  Constraint::Constraint(const Constraint & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  Constraint::~Constraint()
  {
    delete pimpl;
  }
  
  Constraint& Constraint::operator= (const Constraint & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
}
