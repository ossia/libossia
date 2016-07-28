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

#include <ossia/network/v1/Protocol/OSC.hpp>

#include <ossia/network/v1/detail/JamomaProtocol.hpp>

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaOSC final : public OSC, public JamomaProtocol
{

private:

# pragma mark -
# pragma mark Implementation specific

  string    mIp;
  int       mInPort{};            /// the port that a remote device open to receive OSC messages
  int       mOutPort{};           /// the port where a remote device sends OSC messages to give some feeback (like "echo")
  bool      mLearning{};          /// if the device is currently learning from inbound messages.

  TTObject  mNamespaceObserver;
public:

# pragma mark -
# pragma mark Life cycle

  JamomaOSC(string, int, int);

  ~JamomaOSC();

# pragma mark -
# pragma mark Accessors

  std::string getIp() const override;

  Protocol & setIp(std::string) override;

  int getInPort() const override;

  Protocol & setInPort(int) override;

  int getOutPort() const override;

  Protocol & setOutPort(int) override;

  bool getLearningStatus() const override;
  Protocol & setLearningStatus(OSSIA::Device&, bool) override;

# pragma mark -
# pragma mark Operation

  bool updateChildren(Node& node) const override;
};
