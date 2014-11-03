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

namespace OSSIA {

template <typename T>
class Address<T>::Impl {
    
  public:
  
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
  ;
}
  
template <typename T>
Address<T>& Address<T>::getParent() const
{
  ;
}
  
template <typename T>
Device<T>& Address<T>::getDevice() const
{
  ;
}
/*
template <typename T>
const_iterator Address<T>::begin() const
{
  ;
}
  
template <typename T>
const_iterator Address<T>::end() const
{
  ;
}
  
template <typename T>
const_iterator Address<T>::find(const Address<T>&) const
{
  ;
}
*/
template <typename T>
void addChild(const Address<T>&)
 {
   ;
 }
  
template <typename T>
bool removeChild(const Address<T>&)
{
  ;
}

template <typename T>
std::string getAddress()
{
  ;
}

}
