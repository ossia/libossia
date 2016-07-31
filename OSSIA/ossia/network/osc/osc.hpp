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

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <unordered_map>
#include <mutex>

namespace ossia
{
namespace net
{
class OSSIA_EXPORT OSC2 final :
        public ossia::net::protocol_base
{
    private:
        std::string    mIp;
        uint16_t       mInPort{};            /// the port that a remote device opens
        uint16_t       mOutPort{};           /// the port where a remote device sends OSC messages to (opened in this library)
        bool           mLearning{};          /// if the device is currently learning from inbound messages.

        osc::sender    mSender;
        osc::receiver  mReceiver;

        std::mutex mListeningMutex;
        std::unordered_map<std::string, ossia::net::address_base*> mListening;

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
        OSC2 & setLearningStatus(ossia::net::device_base&, bool);


        bool update(ossia::net::node_base& node_base) override;

        bool pull(ossia::net::address_base& address_base) override;

        bool push(const ossia::net::address_base& address_base) override;

        bool observe(ossia::net::address_base& address_base, bool enable) override;

    private:
        void handleReceivedMessage(
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip);
};
}
}
