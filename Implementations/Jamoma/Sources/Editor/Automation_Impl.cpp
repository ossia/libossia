/*!
 * \file Automation_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Automation_Impl.h"

namespace OSSIA
{
  Automation* Automation::create()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
    
    return new Automation_Impl();
  }
  
  Automation_Impl::Automation_Impl()
  {}
  
  Automation_Impl::~Automation_Impl()
  {}
  
  void Automation_Impl::play() const
  {}
  
  class Automation_Impl::const_iterator
  {};
  
  Address::const_iterator Automation_Impl::begin() const
  {}
  
  Address::const_iterator Automation_Impl::end() const
  {}
  
  Address::const_iterator Automation_Impl::find(const Address&) const
  {}
  
  void Automation_Impl::addAddress(const Address&)
  {}
  
  bool Automation_Impl::removeAddress(const Address&)
  {}
  
  TimeValue Automation_Impl::getPeriod() const
  {}
  
  void Automation_Impl::setPeriod(const TimeValue)
  {}
  
  Curve & Automation_Impl::getCurve() const
  {}
  
  void Automation_Impl::setCurve(const Curve&)
  {}
}