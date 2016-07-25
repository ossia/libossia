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
    public virtual OSSIA::v2::Protocol2,
    public BasicProtocol
{

public:
  Local2();

  /*! destructor */
  virtual ~Local2();


  bool pullAddressValue(OSSIA::v2::Address2&) const override;
  bool pushAddressValue(const OSSIA::v2::Address2&) const override;
  bool observeAddressValue(OSSIA::v2::Address2&, bool) const override;
  bool updateChildren(OSSIA::v2::Node2& node) const override;
};
}
