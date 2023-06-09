#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/audio_spin_mutex.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/nullable_variant.hpp>
#include <ossia/detail/small_vector.hpp>

#include <smallfun.hpp>

#include <memory>

/**
 * \file expression_fwd.hpp
 */
namespace ossia
{
namespace expressions
{

/**
 * A callback for expressions.
 */
using expression_result_callback
    = smallfun::function<void(bool), smallfun::DefaultSize * 2>;

class expression_callback_container
{
  using mutex = ossia::audio_spin_mutex;
  using lock_guard = std::lock_guard<ossia::audio_spin_mutex>;

public:
  using T = expression_result_callback;
  expression_callback_container() noexcept = default;
  expression_callback_container(const expression_callback_container& other) = delete;
  expression_callback_container(expression_callback_container&& other) noexcept = delete;
  expression_callback_container& operator=(const expression_callback_container& other)
      = delete;
  expression_callback_container&
  operator=(expression_callback_container&& other) noexcept
      = delete;

  virtual ~expression_callback_container() = default;

  /**
   * @brief impl How the callbackas are stored.
   * A list is used since iterators to other callbacks
   * must not be invalidated upon removal.
   */
  using impl = typename ossia::static_vector<T, 2>;
  using iterator = typename impl::iterator;

  /**
   * @brief add_callback Add a new callback.
   * @param callback must be a std::function or similar.
   * @return iterator to save in order to be able to remove the callback.
   */
  iterator add_callback(T&& callback)
  {
    if(callback.allocated())
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

private:
  impl m_callbacks TS_GUARDED_BY(m_mutx);
  mutable ossia::audio_spin_mutex m_mutx;
};

//using expression_callback_container = callback_container<expression_result_callback>;
using expression_callback_iterator = typename expression_callback_container::iterator;
class expression_atom;
class expression_bool;
class expression_composition;
class expression_not;
class expression_pulse;
class expression_generic;
using expression_base = ossia::nullable_variant<
    expression_atom, expression_bool, expression_composition, expression_not,
    expression_pulse, expression_generic>;

// concept expression_base :
//    public callback_container<expression_result_callback>
//{
// public:

//  /*! evaluate the expression
//   \return bool result of the evaluation */
//  bool evaluate() const;

//  /*! pull the value of any #Destination operand */
//  void update() const;

//};
}
using expression_ptr = std::unique_ptr<expressions::expression_base>;

/**
 * \typedef expression
 *
 * An expression is a formula that checks truth values
 * according to values in a network tree.
 *
 * For instance :
 *
 * \code
 * (a:/b == 50) && !(c:/d < e:/f)
 * \endcode
 *
 * Expressions are built with factories found in \ref expression.hpp :
 * * \ref make_expression_true : a constant "true" node.
 * * \ref make_expression_false : a constant "false" node.
 * * \ref make_expression_atom : a relational comparison node (==, <=, !=,
 * etc...)
 *                               between two \ref ossia::value.
 * * \ref make_expression_pulse : a node that becomes true
 *                                if a message is received opn a given address
 * since the last time
 *                                update() has been called on it.
 * * \ref make_expression_not : standard logical not operator.
 * * \ref make_expression_composition : standard logical and, or, xor,
 * operators
 *                                      between two expressions..
 *
 * \see expression_atom
 * \see expression_bool
 * \see expression_composition
 * \see expression_not
 * \see expression_pulse
 */
using expression = expressions::expression_base;
}
