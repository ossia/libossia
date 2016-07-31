#pragma once

#include <list>

namespace ossia
{
template <typename T>
class callback_container
{
public:
  virtual ~callback_container() = default;

  /*! to store a set of callback functions */
  using impl = typename std::list<T>;
  using iterator = typename impl::const_iterator;

  /*! add a callback function
   \param #T function object */
  iterator addCallback(T callback)
  {
    auto it = mCallbacks.insert(callbacks().begin(), std::move(callback));
    if (mCallbacks.size() == 1)
      onFirstCallbackAdded();
    return it;
  }

  /*! remove a result callback function
   \param #it Iterator to remove */
  void removeCallback(iterator it)
  {
    if (mCallbacks.size() == 1)
      onRemovingLastCallback();
    mCallbacks.erase(it);
  }

  /*! get callback functions
   \return #CallbackList */
  impl& callbacks()
  {
    return mCallbacks;
  }

  /*! get callback functions
   \return #CallbackList */
  const impl& callbacks() const
  {
    return mCallbacks;
  }

  /*! trigger all callbacks
   \param #args arguments to all callbacks */
  template <typename... Args>
  void send(Args&&... args)
  {
    for (auto& callback : mCallbacks)
      callback(std::forward<Args>(args)...);
  }

  auto begin()
  {
    return mCallbacks.begin();
  }
  auto end()
  {
    return mCallbacks.end();
  }
  auto begin() const
  {
    return mCallbacks.begin();
  }
  auto end() const
  {
    return mCallbacks.end();
  }
  auto empty() const
  {
    return mCallbacks.empty();
  }

  void clear()
  {
    onRemovingLastCallback();
    mCallbacks.clear();
  }

protected:
  virtual void onFirstCallbackAdded()
  {
  }
  virtual void onRemovingLastCallback()
  {
  }
  impl mCallbacks;
};

template <typename T>
typename callback_container<T>::iterator begin(callback_container<T>& cont)
{
  return cont.callbacks().begin();
}

template <typename T>
typename callback_container<T>::iterator end(callback_container<T>& cont)
{
  return cont.callbacks().end();
}

template <typename T>
typename callback_container<T>::iterator
cbegin(const callback_container<T>& cont)
{
  return cont.callbacks().cbegin();
}

template <typename T>
typename callback_container<T>::iterator
cend(const callback_container<T>& cont)
{
  return cont.callbacks().cend();
}
}
