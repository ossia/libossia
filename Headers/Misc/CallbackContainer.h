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
  virtual iterator addCallback(T callback)
  {
    return m_callbacks.insert(callbacks().cend(), std::move(callback));
  }

  /*! remove a result callback function
   \param #it Iterator to remove */
  virtual void removeCallback(iterator it)
  {
    if (it == m_callbacks.end())
      return;
    m_callbacks.erase(it);
  }

  /*! get callback functions
   \return #CallbackList */
  ContainerImpl& callbacks()
  { return m_callbacks; }

  /*! get callback functions
   \return #CallbackList */
  const ContainerImpl& callbacks() const
  { return m_callbacks; }

  /*! trigger all callbacks
   \param #args arguments to all callbacks */
  template<typename... Args>
  void send(Args&&... args)
  {
    for (auto callback : m_callbacks)
        callback(std::forward<Args>(args)...);
  }

protected:
  ContainerImpl m_callbacks;

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
