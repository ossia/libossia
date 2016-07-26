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

#include <ossia/network/v1/Protocol.hpp>

namespace OSSIA
{

class OSSIA_EXPORT Local : public virtual Protocol
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \return std::shared_ptr<Local> */
  static std::shared_ptr<Local> create();

  /*! destructor */
  virtual ~Local();

};

}

