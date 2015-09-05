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

# pragma mark -
# pragma mark Life cycle

  /*! destructor */
  virtual ~CallbackContainer() = default;

# pragma mark -
# pragma mark Callback

  /*! to store a set of callback functions */
  using CallbackVector = typename std::list<T>;
  using iterator = typename CallbackVector::const_iterator;

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
     m_callbacks.erase(it);
   }

  /*! get callback functions
   \return #CallbackList */
  CallbackVector& callbacks()
  { return m_callbacks; }

  /*! get callback functions
   \return #CallbackList */
  const CallbackVector& callbacks() const
  { return m_callbacks; }

protected:
  CallbackVector m_callbacks;

};

}
