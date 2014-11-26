/*!
 * \file Message_Impl.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef MESSAGE_IMPL_H_
#define MESSAGE_IMPL_H_

#include "Editor/Message.h"

#include "TTScore.h"

namespace OSSIA
{
  class Message_Impl : public Message
  {
  public:
    
    // Constructors, destructor, assignment
    Message_Impl();
    virtual ~Message_Impl();
    
    /** Implementation Specific
     @details use mutable members to break constness of the API because Jamoma doesn't take care of it.
     */
  };
}

#endif /* MESSAGE_IMPL_H_ */
