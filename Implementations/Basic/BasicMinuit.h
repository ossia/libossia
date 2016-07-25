/*!
 * \file JamomaMinuit.h
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

#include <string>

#include "Network/Protocol/Minuit.h"

#include "BasicNode.h"
#include "BasicProtocol.h"


namespace impl
{
class Minuit2 final : public BasicProtocol
{
private:
  string    mIp;
  int       mInPort;            /// the port that a remote device open to receive OSC messages
  int       mOutPort;           /// the port where a remote device sends OSC messages to give some feeback (like "echo")

public:
  Minuit2(string, int, int);

  ~Minuit2();

  std::string getIp();
  Minuit2 & setIp(std::string);

  int getInPort();
  Minuit2 & setInPort(int);

  int getOutPort();
  Minuit2 & setOutPort(int);

  bool updateChildren(Node& node) const override;
};
}
