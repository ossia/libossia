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

#include "BasicProtocol.h"
#include "BasicNode.h"

namespace OSSIA
{

class Protocol2;

class OSSIA_EXPORT Device2 : public virtual Node2
{

public:
  virtual ~Device();

  virtual Protocol2* getProtocol() const = 0;
  virtual bool updateNamespace() = 0;
};
}

namespace impl
{

class BasicDevice final :
    public Device2,
    public BasicNode
{

private:
  unique_ptr<Protocol2>  mProtocol;

public:
  BasicDevice(unique_ptr<Protocol2> protocol);

  ~BasicDevice();

  std::string getName() const override;
  OSSIA::Node2 & setName(std::string) override;

  Protocol2* getProtocol() const override;
  bool updateNamespace() override;
};
}
