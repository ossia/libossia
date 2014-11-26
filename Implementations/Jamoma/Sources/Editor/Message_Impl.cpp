/*!
 * \file Message_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Message_Impl.h"

namespace OSSIA
{
    Message* Message::create(Address address, AddressType type)
    {
        // todo : we shouldn't init each time we create an object ...
        TTFoundationInit("/usr/local/jamoma/");
        TTModularInit("/usr/local/jamoma/");
        TTScoreInit("/usr/local/jamoma/");
        
        return new Message_Impl(address, type);
    }
    
    Message_Impl::Message_Impl(Address address, AddressType type)
    {}
    
    Message_Impl::~Message_Impl()
    {}
    
    Address & Message_Impl::getAddress() const
    {
      TTAddress address;
      TTValue v;
      
      mDictionary.lookup(kTTSym_address, v);
      address = v[0];
      
      return address;
    }
    
    AddressType Message_Impl::getValue() const
    {
      TTValue v;
      mDictionary.getValue(v);
      
      
      return;
    }
}