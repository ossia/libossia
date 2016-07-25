/*!
 * \file JamomaOSC.h
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

#include "Network/Protocol/OSC.h"

#include "BasicProtocol.h"

namespace impl
{
class OSC2 final : public BasicProtocol
{
private:
  std::string    mIp;
  int       mInPort{};            /// the port that a remote device open to receive OSC messages
  int       mOutPort{};           /// the port where a remote device sends OSC messages to give some feeback (like "echo")
  bool      mLearning{};          /// if the device is currently learning from inbound messages.

public:
  OSC2(string, int, int);
  ~OSC2();


  std::string getIp() const override;
  OSC2 & setIp(std::string) override;

  int getInPort() const override;
  OSC2 & setInPort(int) override;

  int getOutPort() const override;
  OSC2 & setOutPort(int) override;

  bool getLearningStatus() const override;
  OSC2 & setLearningStatus(OSSIA::Device&, bool) override;


  bool updateChildren(Node& node) const override;
};
}
