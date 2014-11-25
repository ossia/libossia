/*!
 * \file Message.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Message.h"

#include "TTScore.h"

namespace OSSIA
{
  template <typename T>
  class Message<T>::Impl
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
  
  template <typename T>
  Message<T>::Message() :
  pimpl(new Impl)
  {}
  
  template <typename T>
  Message<T>::Message(const Message & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  template <typename T>
  Message<T>::~Message()
  {
    delete pimpl;
  }
  
  template <typename T>
  Message<T>& Message<T>::operator= (const Message & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
}