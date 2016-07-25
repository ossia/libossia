/*!
 * \file JamomaDevice.h
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

#include "Device.hpp"
#include "BasicNode.h"

namespace impl
{

class BasicDevice final :
    public OSSIA::v2::Device2,
    public BasicNode
{

private:
  std::unique_ptr<OSSIA::v2::Protocol2>  mProtocol;

public:
  BasicDevice(std::unique_ptr<OSSIA::v2::Protocol2> protocol);

  ~BasicDevice();

  OSSIA::v2::Protocol2& getProtocol() const override;
  bool updateNamespace() override;
};
}
