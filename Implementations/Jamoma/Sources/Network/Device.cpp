/*!
 * \file Device.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Network/Device.h"

#include "TTModular.h"

namespace OSSIA {
  
class SharedImpl {
    
public:
  
  TTObject mApplicationManager;
  TTObject mApplication;
    
  SharedImpl()
  {
    // todo : move this else where ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    
    // if no application manager
    if (TTModularApplicationManager == NULL)
      mApplicationManager = TTObject("ApplicationManager");
    else
      mApplicationManager = TTObjectBasePtr(TTModularApplicationManager);
  };
  
  SharedImpl(const SharedImpl & other) = default;
  ~SharedImpl() = default;
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
Device<T>::Device(T * protocol) :
pimpl(new Impl)
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
bool Device<T>::save(std::string filepath) const
{
  // create a xml handler
  TTObject aXmlHandler(kTTSym_XmlHandler);
  
  // pass it the application
  aXmlHandler.set(kTTSym_object, pimpl->mApplication);
  
  // write
  TTValue none;
  TTErr err = aXmlHandler.send(kTTSym_Write, filepath, none);
  
  return err == kTTErrNone;
}
  
template <typename T>
bool Device<T>::load(std::string filepath) const
{
  // create a xml handler
  TTObject aXmlHandler(kTTSym_XmlHandler);
  
  // pass it the application
  aXmlHandler.set(kTTSym_object, pimpl->mApplication);
  
  // read
  TTValue none;
  TTErr err = aXmlHandler.send(kTTSym_Read, filepath, none);
  
  return err == kTTErrNone;
}

}
