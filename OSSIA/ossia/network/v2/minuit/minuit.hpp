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

#include "BasicNode.hpp"
#include "Protocol.hpp"


namespace impl
{
class Minuit2 final : public OSSIA::v2::Protocol
{
    private:
        std::string    mIp;
        int            mInPort;  /// the port that a remote device open to receive OSC messages
        int            mOutPort; /// the port where a remote device sends OSC messages to give some feeback (like "echo")

    public:
        Minuit2(std::string, int, int);

        ~Minuit2();

        std::string getIp() const;
        Minuit2 & setIp(std::string);

        int getInPort() const;
        Minuit2 & setInPort(int);

        int getOutPort() const;
        Minuit2 & setOutPort(int);

        bool updateChildren(OSSIA::v2::Node& node) const override;

        bool pullAddressValue(OSSIA::v2::Address& address) const override;

        bool pushAddressValue(const OSSIA::v2::Address& address) const override;

        bool observeAddressValue(OSSIA::v2::Address& address, bool enable) const override;
};
}
