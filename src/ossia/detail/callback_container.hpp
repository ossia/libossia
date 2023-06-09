#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/mutex.hpp>

#if defined(__cpp_exceptions)
#include <exception>
#endif
#include <list>
#include <mutex>

/**
 * \file callback_container.hpp
 */

namespace ossia
{

#if defined(__cpp_exceptions)
/**
 * @brief The invalid_callback_error class
 *
 * Means that an invalid callback was passed
 */
struct OSSIA_EXPORT invalid_callback_error final : public std::exception
{
  ~invalid_callback_error() override;
  invalid_callback_error();
  const char* what() const noexcept override;
};
#endif

template <typename T>
/**
 * @brief The callback_container class
 *
 * Contains callbacks.
 * Classes that have network callbacks may want to derive from this and
 * implement
 * on_first_callback_added, on_removing_last_callback.
 *
 * This allows to cleanly stop listening when there are no callbacks.
 *
 */
class callback_container
{
  using mutex = OSSIA_CALLBACK_CONTAINER_MUTEX;
  using lock_guard = std::lock_guard<mutex>;

public:
  callback_container() = default;
  callback_container(const callback_container& other)
  {
    lock_guard lck{other.m_mutx};
    m_callbacks = other.m_callbacks;
  }
  callback_container(callback_container&& other) noexcept
  {
    lock_guard lck{other.m_mutx};
    m_callbacks = std::move(other.m_callbacks);
  }
  callback_container& operator=(const callback_container& other)
  {
    lock_guard lck{other.m_mutx};
    m_callbacks = other.m_callbacks;
    return *this;
  }
  callback_container& operator=(callback_container&& other) noexcept
  {
    lock_guard lck{other.m_mutx};
    m_callbacks = std::move(other.m_callbacks);
    return *this;
  }

  virtual ~callback_container() = default;

  /**
   * @brief impl How the callbackas are stored.
   * A list is used since iterators to other callbacks
   * must not be invalidated upon removal.
   */
  using impl = std::list<T>;
  using iterator = typename impl::iterator;

  /**
   * @brief add_callback Add a new callback.
   * @param callback must be a std::function or similar.
   * @return iterator to save in order to be able to remove the callback.
   */
  iterator add_callback(T&& callback)
  {
    if(callback)
    {
      lock_guard lck{m_mutx};
      auto it = m_callbacks.insert(m_callbacks.begin(), std::move(callback));
      if(m_callbacks.size() == 1)
        on_first_callback_added();
      return it;
    }
    else
    {
#if defined(__cpp_exceptions)
      throw invalid_callback_error{};
#else
      return {};
#endif
    }
  }

  /**
   * @brief remove_callback Removes a callback identified by an iterator.
   * @param it Iterator to remove.
   */
  void remove_callback(iterator it)
  {
    lock_guard lck{m_mutx};
    if(m_callbacks.size() == 1)
      on_removing_last_callback();

    m_callbacks.erase(it);
  }

  /**
   * @brief Replaces an existing callback with another function.
   */
  void replace_callback(iterator it, T&& cb)
  {
    lock_guard lck{m_mutx};
    *m_callbacks.erase(it, it) = std::move(cb);
  }

  void replace_callbacks(impl&& cbs)
  {
    lock_guard lck{m_mutx};
    m_callbacks = std::move(cbs);
  }

  class disabled_callback
  {
  public:
    explicit disabled_callback(callback_container& self)
        : self{self}
        , old_callbacks{self.m_callbacks}
    {
    }

    ~disabled_callback() { self.replace_callbacks(std::move(old_callbacks)); }

  private:
    callback_container& self;
    callback_container::impl old_callbacks;
  };

  disabled_callback disable_callback(iterator it)
  {
    lock_guard lck{m_mutx};
    disabled_callback dis{*this};

    // TODO should we also call on_removing_last_blah ?
    // I don't think so : it's supposed to be a short operation
    m_callbacks.erase(it);
    return dis;
  }

  /**
   * @brief callback_count
   * @return Number of active callbacks.
   */
  std::size_t callback_count() const
  {
    lock_guard lck{m_mutx};
    return m_callbacks.size();
  }

  /**
   * @brief callbacks_empty
   * @return True if there are no callbacks
   */
  bool callbacks_empty() const
  {
    lock_guard lck{m_mutx};
    return m_callbacks.empty();
  }

  /**
   * @brief send Trigger all callbacks
   * @param args Arguments to send to the callbacks.
   */
  template <typename... Args>
  void send(Args&&... args)
  {
    lock_guard lck{m_mutx};
    for(auto& callback : m_callbacks)
    {
      if(callback)
        callback(std::forward<Args>(args)...);
    }
  }

  /**
   * @brief clear Clears callbacks.
   */
  void callbacks_clear()
  {
    lock_guard lck{m_mutx};
    if(!m_callbacks.empty())
      on_removing_last_callback();
    m_callbacks.clear();
  }

protected:
  /**
   * @brief on_first_callback_added
   *
   * These functions can be reimplemented by
   * subclasses wishing to perform special actions.
   * For instance, when the last callback is removed,
   * stop listening somewhere...
   *
   * \see \ref on_removing_last_callback
   */
  virtual void on_first_callback_added() { }

  /**
   * @brief on_removing_last_callback
   *
   * \see \ref on_first_callback_added
   */
  virtual void on_removing_last_callback() { }

  //private:
public:
  impl m_callbacks TS_GUARDED_BY(m_mutx);
  mutable mutex m_mutx;
};

}
