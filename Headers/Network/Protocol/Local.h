/*!
 * \file Local.h
 *
 * \defgroup Network
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Network/Protocol.h"

namespace OSSIA
{

class Local : public virtual Protocol
{
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \return std::shared_ptr<Local> */
  static std::shared_ptr<Local> create();
  
  /*! destructor */
  virtual ~Local();
  
# pragma mark -
# pragma mark Accessors
  
  Protocol::Type getType() const override final
  {return Protocol::Type::LOCAL;}

};

}

