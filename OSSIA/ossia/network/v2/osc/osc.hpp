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

#include <ossia/network/v2/base/protocol.hpp>
#include <ossia/network/v2/osc/detail/sender.hpp>
#include <ossia/network/v2/osc/detail/receiver.hpp>
#include <unordered_map>
#include <mutex>

namespace impl
{
class OSSIA_EXPORT OSC2 final :
        public OSSIA::v2::Protocol
{
    private:
        std::string    mIp;
        uint16_t       mInPort{};            /// the port that a remote device opens
        uint16_t       mOutPort{};           /// the port where a remote device sends OSC messages to (opened in this library)
        bool           mLearning{};          /// if the device is currently learning from inbound messages.

        osc::sender    mSender;
        osc::receiver  mReceiver;

        std::mutex mListeningMutex;
        std::unordered_map<std::string, OSSIA::v2::Address*> mListening;

    public:
        OSC2(std::string, uint16_t, uint16_t);
        ~OSC2();


        const std::string& getIp() const;
        OSC2 & setIp(std::string);

        uint16_t getInPort() const;
        OSC2 & setInPort(uint16_t);

        uint16_t getOutPort() const;
        OSC2 & setOutPort(uint16_t);

        bool getLearningStatus() const;
        OSC2 & setLearningStatus(OSSIA::v2::Device&, bool);


        bool updateChildren(OSSIA::v2::Node& node) override;

        bool pullAddressValue(OSSIA::v2::Address& address) override;

        bool pushAddressValue(const OSSIA::v2::Address& address) override;

        bool observeAddressValue(OSSIA::v2::Address& address, bool enable) override;

    private:
        void handleReceivedMessage(
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip);
};
}
