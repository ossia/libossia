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

#include <future>
#include <string>
#include <set>
#include <ossia/network/v2/generic/generic_node.hpp>
#include <ossia/network/v2/base/protocol.hpp>
#include <ossia/network/v2/osc/detail/sender.hpp>
#include <ossia/network/v2/osc/detail/receiver.hpp>
#include <ossia/network/v2/domain.hpp>

#include <ossia/network/v2/minuit/detail/minuit_name_table.hpp>

#include <ossia/editor/value/value.hpp>
#include <unordered_map>
#include <mutex>

namespace impl
{
class BasicDevice;
class OSSIA_EXPORT Minuit2 final : public OSSIA::v2::Protocol
{
    private:
        std::string    mIp;
        uint16_t       mInPort{};            /// the port that a remote device opens
        uint16_t       mOutPort{};           /// the port where a remote device sends OSC messages to (opened in this library)
        bool           mLearning{};          /// if the device is currently learning from inbound messages.

        std::mutex mListeningMutex;
        std::unordered_map<std::string, OSSIA::v2::Address*> mListening;

        std::promise<void> mNamespacePromise;
        impl::BasicDevice* mDevice;

        std::set<std::string, std::less<>> m_namespaceRequests;
    public:
        osc::sender    mSender;
        OSSIA::minuit::name_table mNameTable;

        Minuit2(std::string, uint16_t, uint16_t);
        ~Minuit2();

        void setDevice(OSSIA::v2::Device& dev) override;

        const std::string& getIp() const;
        Minuit2& setIp(std::string);

        uint16_t getInPort() const;
        Minuit2& setInPort(uint16_t);

        uint16_t getOutPort() const;
        Minuit2& setOutPort(uint16_t);

        bool updateChildren(OSSIA::v2::Node& node) override;

        bool pullAddressValue(OSSIA::v2::Address& address) override;

        bool pushAddressValue(const OSSIA::v2::Address& address) override;

        bool observeAddressValue(OSSIA::v2::Address& address, bool enable) override;

        void refresh(boost::string_ref req, const std::string& addr)
        {
          auto it = m_namespaceRequests.find(addr);
          if(it == m_namespaceRequests.end())
          {
            m_namespaceRequests.insert(addr);
            mSender.send(req, boost::string_ref{addr});
          }
        }

        void refreshed(boost::string_ref addr)
        {
          auto it = m_namespaceRequests.find(addr);
          if(it != m_namespaceRequests.end())
          {
            m_namespaceRequests.erase(it);
          }

          if(m_namespaceRequests.empty())
          {
            mNamespacePromise.set_value();
          }
        }

    private:
        void handleReceivedMessage(
                const oscpack::ReceivedMessage& m,
                const oscpack::IpEndpointName& ip);

        void on_change(string_view address, OSSIA::Value);
        void on_change(string_view address, OSSIA::v2::Domain);
        void on_change(string_view address, OSSIA::AccessMode);
        void on_change(string_view address, OSSIA::BoundingMode);
        void on_change(string_view address, OSSIA::RepetitionFilter repfilter);


        osc::receiver  mReceiver;


};
}
