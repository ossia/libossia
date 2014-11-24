/*!
 * \file Protocols.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Network/Protocols.h"

#include "TTModular.h"

namespace OSSIA {
    
class Local::Impl {
    
public:
    
    Impl()
    {
        ;
    };
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
    
};

Local::Local(std::string name)
{
    ;
}

Local::~Local()
{
    ;
}
    

class Minuit::Impl {
  
public:
  
  Impl()
  {
    ;
  };
  
  Impl(const Impl & other) = default;
  ~Impl() = default;
  
};

Minuit::Minuit(std::string name, std::string ip, int port)
{
  ;
}

Minuit::~Minuit()
{
  ;
}

  
class OSC::Impl {
  
public:
  
  Impl()
  {
    ;
  };
  
  Impl(const Impl & other) = default;
  ~Impl() = default;
  
};

OSC::OSC(std::string ip, int in_port, int out_port)
{
  ;
}

OSC::~OSC()
{
  ;
}
    
}
