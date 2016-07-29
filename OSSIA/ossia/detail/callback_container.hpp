/*!
 * \file CallbackContainer.h
 *
 * \defgroup Misc
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <list>

namespace OSSIA
{
template <typename T>
class CallbackContainer
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! destructor */
  virtual ~CallbackContainer() = default;

#if 0
# pragma mark -
# pragma mark Callback
#endif

  /*! to store a set of callback functions */
  using ContainerImpl = typename std::list<T>;
  using iterator = typename ContainerImpl::const_iterator;

  /*! add a callback function
   \param #T function object */
  iterator addCallback(T callback)
  {
    auto it = mCallbacks.insert(callbacks().begin(), std::move(callback));
    if(mCallbacks.size() == 1)
      onFirstCallbackAdded();
    return it;
  }

  /*! remove a result callback function
   \param #it Iterator to remove */
  void removeCallback(iterator it)
  {
    if(mCallbacks.size() == 1)
      onRemovingLastCallback();
    mCallbacks.erase(it);
  }

  /*! get callback functions
   \return #CallbackList */
  ContainerImpl& callbacks()
  { return mCallbacks; }

  /*! get callback functions
   \return #CallbackList */
  const ContainerImpl& callbacks() const
  { return mCallbacks; }

  /*! trigger all callbacks
   \param #args arguments to all callbacks */
  template<typename... Args>
  void send(Args&&... args)
  {
    for (auto& callback : mCallbacks)
      callback(std::forward<Args>(args)...);
  }

  auto begin() { return mCallbacks.begin(); }
  auto end() { return mCallbacks.end(); }
  auto begin() const { return mCallbacks.begin(); }
  auto end() const { return mCallbacks.end(); }
  auto empty() const { return mCallbacks.empty(); }

  void clear()
  {
    onRemovingLastCallback();
    mCallbacks.clear();
  }

protected:
  virtual void onFirstCallbackAdded() { }
  virtual void onRemovingLastCallback() { }
  ContainerImpl mCallbacks;

};

template<typename T>
typename CallbackContainer<T>::iterator
begin(CallbackContainer<T>& cont)
{
  return cont.callbacks().begin();
}

template<typename T>
typename CallbackContainer<T>::iterator
end(CallbackContainer<T>& cont)
{
  return cont.callbacks().end();
}

template<typename T>
typename CallbackContainer<T>::iterator
cbegin(const CallbackContainer<T>& cont)
{
  return cont.callbacks().cbegin();
}

template<typename T>
typename CallbackContainer<T>::iterator
cend(const CallbackContainer<T>& cont)
{
  return cont.callbacks().cend();
}

}
