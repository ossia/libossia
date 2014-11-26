/*!
 * \file Address_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Address_Impl.h"

namespace OSSIA
{
  Address* Address::create()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
    
    return new Address_Impl();
  }
  
  Address_Impl::Address_Impl()
  {}
  
  Address_Impl::~Address_Impl()
  {}
  
  Address Address::addAddress(std::string) const
  {}
  
  bool Address_Impl::isRoot() const
  {}
  
  Address & Address_Impl::getParent() const
  {}
  
  Device<T> & Address_Impl::getDevice() const
  {}
  
  class Address_Impl::const_iterator
  {};
  
  Address::const_iterator Address_Impl::begin() const
  {}
  
  Address::const_iterator Address_Impl::end() const
  {}
  
  Address::const_iterator Address_Impl::find(const Address&) const
  {}
  
  void Address_Impl::addChild(const Address&)
  {}
  
  bool Address_Impl::removeChild(const Address&)
  {}
  
  std::string Address_Impl::getAddress() const
  {}
}