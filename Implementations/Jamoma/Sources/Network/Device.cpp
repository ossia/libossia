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

#include "TTModular.h"

namespace OSSIA {
  
template <typename T>
class Device<T>::Impl {
    
public:
  
  TTObject mApplication;
    
  Impl(std::string name, bool local)
  {
    // todo : move this else where ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    
    // if no application manager
    TTObject applicationManager;
    if (TTModularApplicationManager == NULL)
      applicationManager = TTObject("ApplicationManager");
    else
      applicationManager = TTObjectBasePtr(TTModularApplicationManager);
    
    // create local or distante application
    TTErr   err;
    TTValue out;
    if (local)
      err = applicationManager.send("ApplicationInstantiateLocal", name, out);
    else
      err = applicationManager.send("ApplicationInstantiateDistante", name, out);
    
    if (err) {
      TTLogError("Error : can't create %s application\n", name);
      return;
    }
    else
      mApplication = out[0];
    
    // set the application in debug mode
    mApplication.set("debug", YES);
  };
  
  Impl(const Impl & other) = default;
  ~Impl() = default;
  
  bool isLocal()
  {
    return mApplication.instance() == TTObjectBasePtr(accessApplicationLocal);
  }
  
  bool registerDataAtAddress(TTAddress& anAddress, TTSymbol& service, TTObject& returnedData)
  {
    // TODO : allow to add address into the local application
    if (isLocal())
      return false;
    
    // create a proxy data
    TTValue out, v(anAddress, service);
    if (!mApplication.send("ProxyDataInstantiate", v, out)) {
      
      returnedData = out[0];
      
      // initialize the value with a default 0. value
      returnedData.set("valueDefault", 0.);
      
      returnedData.send("Init");
      
      return true;
    }
    
    return false;
  }
};

template <typename T>
Device<T>::Device() :
pimpl(new Impl) // TODO : pass a name and precise if it is local or not
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
Address<T> Device<T>::addAddress(std::string address) const
{
  TTAddress anAddress(address.data());
  TTSymbol  service("parameter");
  TTObject  aData;
  
  if (pimpl->registerDataAtAddress(anAddress, service, aData))
  {
    // TODO : setup attribute depending on the T type
  }
}
  
template <typename T>
template <typename U>
Address<T> Device<T>::addAddress(std::string address) const
{
  TTAddress anAddress(address.data());
  TTSymbol  service("parameter");
  TTObject  aData;
  
  if (pimpl->registerDataAtAddress(anAddress, service, aData))
  {
    // TODO : setup attribute depending on the U type
  }
}
  
template <typename T>
template <typename U>
Address<T> Device<T>::addAddress(std::string address, U min, U max) const
{
  TTAddress anAddress(address.data());
  TTSymbol  service("parameter");
  TTObject  aData;
  
  if (pimpl->registerDataAtAddress(anAddress, service, aData))
  {
    // TODO : setup attribute depending on the U type
  }
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
