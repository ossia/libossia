/*!
 * \file Device.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Network/Device.h"

namespace OSSIA {
  
template <typename T>
class Device<T>::Impl {
    
public:
    
  Impl()
  {
    ;
  };
    
  Impl(const Impl & other) = default;
  ~Impl() = default;
    
};

template <typename T>
Device<T>::Device() :
pimpl(new Impl)
{}
  
template <typename T>
Device<T>::Device(const Device & other) :
pimpl(new Impl(*(other.pimpl)))
{}
  
template <typename T>
Device<T>::~Device()
{
  delete pimpl;
}
  
template <typename T>
Device<T>& Device<T>::operator= (const Device & other)
{
  delete pimpl;
  pimpl = new Impl(*(other.pimpl));
  return *this;
}

template <typename T>
Address<T> Device<T>::addAddress(std::string) const
{
  ;
}
  
template <typename T>
template <typename U>
Address<T> Device<T>::addAddress(std::string) const
{
  ;
}
  
template <typename T>
template <typename U>
Address<T> Device<T>::addAddress(std::string, U min, U max) const
{
  ;
}

template <typename T>
bool Device<T>::save(std::string) const
{
  ;
}
  
template <typename T>
bool Device<T>::load(std::string) const
{
  ;
}

}
