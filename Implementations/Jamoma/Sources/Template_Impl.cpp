/*!
 * \file Template_Impl.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Template_Impl.h"

namespace OSSIA
{
  Template* Template::create()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
    
    return new Template_Impl();
  }
  
  Template_Impl::Template_Impl()
  {}
  
  Template_Impl::~Template_Impl()
  {}
  
  void template_feature()
  {}
}