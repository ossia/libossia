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
  Message* Message::create()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
    
    return new Message_Impl();
  }
  
  Message_Impl::Message_Impl()
  {}
  
  Message_Impl::~Message_Impl()
  {}
}