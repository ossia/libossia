#pragma once

#include <list>

namespace ossia
{

template <typename T>
/**
 * @brief The callback_container class
 *
 * Contains callbacks.
 * Classes that have network callbacks may want to derive from this and implement
 * onFirstCallbackAdded, onRemovingLastCallback.
 *
 * This allows to cleanly stop listening when there are no callbacks.
 *
 */
class callback_container
{
public:
  virtual ~callback_container() = default;

  /**
   * @brief impl How the callbackas are stored.
   * A list is used since iterators to other callbacks
   * must not be invalidated upon removal.
   */
  using impl = typename std::list<T>;
  using iterator = typename impl::const_iterator;

  /**
   * @brief add_callback Add a new callback.
   * @param callback must be a std::function or similar.
   * @return iterator to save in order to be able to remove the callback.
   */
  iterator add_callback(T callback)
  {
    auto it = mCallbacks.insert(mCallbacks.begin(), std::move(callback));
    if (mCallbacks.size() == 1)
      onFirstCallbackAdded();
    return it;
  }

  /**
   * @brief remove_callback Removes a callback identified by an iterator.
   * @param it Iterator to remove.
   */
  void remove_callback(iterator it)
  {
    if (mCallbacks.size() == 1)
      onRemovingLastCallback();
    mCallbacks.erase(it);
  }

  /**
   * @brief callback_count
   * @return Number of active callbacks.
   */
  std::size_t callback_count() const
  { return mCallbacks.size(); }

  /**
   * @brief callbacks_empty
   * @return Number of active callbacks.
   */
  std::size_t callbacks_empty() const
  { return mCallbacks.size(); }

  /**
   * @brief send Trigger all callbacks
   * @param args Arguments to send to the callbacks.
   */
  template <typename... Args>
  void send(Args&&... args)
  {
    for (auto& callback : mCallbacks)
      callback(std::forward<Args>(args)...);
  }

  /**
   * @brief clear Clears callbacks.
   */
  void callbacks_clear()
  {
    if(!mCallbacks.empty())
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

private:
  impl mCallbacks;
};

}
