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

#include "Protocol.hpp"

namespace impl
{
class OSC2 final :
        public OSSIA::v2::Protocol
{
    private:
        std::string    mIp;
        int            mInPort{};            /// the port that a remote device open to receive OSC messages
        int            mOutPort{};           /// the port where a remote device sends OSC messages to give some feeback (like "echo")
        bool           mLearning{};          /// if the device is currently learning from inbound messages.

    public:
        OSC2(std::string, int, int);
        ~OSC2();


        std::string getIp() const;
        OSC2 & setIp(std::string);

        int getInPort() const;
        OSC2 & setInPort(int);

        int getOutPort() const;
        OSC2 & setOutPort(int);

        bool getLearningStatus() const;
        OSC2 & setLearningStatus(OSSIA::v2::Device&, bool);


        bool updateChildren(OSSIA::v2::Node& node) const override;

        bool pullAddressValue(OSSIA::v2::Address& address) const override;

        bool pushAddressValue(const OSSIA::v2::Address& address) const override;

        bool observeAddressValue(OSSIA::v2::Address& address, bool enable) const override;
};
}
