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

#include <vector>

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
  using CallbackVector = typename std::vector<T*>;
  
  /*! add a callback function
   \param #T function pointer */
  virtual void addCallback(T*) = 0;
  
  /*! remove a result callback function
   \param #T function pointer */
  virtual void removeCallback(T*) = 0;
  
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
