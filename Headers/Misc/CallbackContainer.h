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
  using CallbackList = typename std::list<T>;
  
  /*! to retreive a callback function into CallbackList */
  using CallbackIterator = typename CallbackList::iterator;
  
  /*! add a callback function
   \param #T function
   \return #CallbackIterator where the function is stored */
  virtual CallbackIterator addCallback(T) = 0;
  
  /*! remove a result callback function
   \param #CallbackIterator where the function is stored
   \return #Callback function */
  virtual void removeCallback(CallbackIterator) = 0;
  
  /*! get callback functions
   \return #CallbackList */
  CallbackList& callbacks()
  { return m_callbacks; }
  
  /*! get callback functions
   \return #CallbackList */
  const CallbackList& callbacks() const
  { return m_callbacks; }
  
protected:
  CallbackList m_callbacks;
  
};
  
}
