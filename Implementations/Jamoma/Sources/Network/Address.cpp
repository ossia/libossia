/*!
 * \file Address.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Network/Address.h"

#include "TTModular.h"

namespace OSSIA {

template <typename T>
class Address<T>::Impl {
    
  public:
  
  TTAddress mAddress;
  
  Impl()
  {
    ;
  };
  
  Impl(const Impl & other) = default;
  ~Impl() = default;

};
  
template <typename T>
Address<T>::Address() :
pimpl(new Impl)
{}

template <typename T>
Address<T>::Address(const Address & other) :
pimpl(new Impl(*(other.pimpl)))
{}

template <typename T>
Address<T>::~Address()
{
  delete pimpl;
}

template <typename T>
Address<T>& Address<T>::operator= (const Address & other)
{
  delete pimpl;
  pimpl = new Impl(*(other.pimpl));
  return *this;
}
  
template <typename T>
Address<T> Address<T>::addAddress(std::string) const
{
  ;
}

template <typename T>
template <typename U>
Address<T> Address<T>::addAddress(std::string) const
{
  ;
}
  
template <typename T>
template <typename U>
Address<T> Address<T>::addAddress(std::string, U min, U max) const
{
  ;
}

template <typename T>
bool Address<T>::isRoot() const
{
  return pimpl->mAddress == kTTAdrsRoot;
}
  
template <typename T>
Address<T>& Address<T>::getParent() const
{
  // théo : one way to do this but there is no such a constructor
  //return Address(pimpl->mAddress.getParent().c_str());
}
  
template <typename T>
Device<T>& Address<T>::getDevice() const
{
  ; // théo : how to retreive the device ? should we have it as a member of the class ?
}

template <typename T>
void Address<T>::addChild(const Address<T>&)
 {
   ; // théo : here do we need the device too ?
 }
  
template <typename T>
bool Address<T>::removeChild(const Address<T>&)
{
  ; // théo : here do we need the device too ?
}

template <typename T>
std::string Address<T>::getAddress()
{
  return pimpl->mAddress.c_str();
}

}
