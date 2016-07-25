/*!
 * \file JamomaLocal.h
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

#include "BasicProtocol.h"


namespace impl
{
class OSSIA_EXPORT Local2 :
    public virtual OSSIA::Protocol2,
    public BasicProtocol
{

public:
  Local2();

  /*! destructor */
  virtual ~Local2();


  bool pullAddressValue(OSSIA::Address2&) const override;
  bool pushAddressValue(const OSSIA::Address2&) const override;
  bool observeAddressValue(OSSIA::Address2&, bool) const override;
  bool updateChildren(OSSIA::Node2& node) const override;
};
}
