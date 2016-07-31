/*!
 * \file JamomaAddress.h
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
#include <ossia/network/base/address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/detail/callback_container.hpp>

#include <thread>
#include <mutex>

namespace ossia
{
namespace net
{
class protocol_base;
class OSSIA_EXPORT generic_address final : public ossia::net::address_base
{
    private:
        const ossia::net::node_base& mNode;
        ossia::net::protocol_base& mProtocol;

        mutable std::mutex mValueMutex;
        ossia::value mValue;

        ossia::net::domain mDomain;

        ossia::net::value_callback mCallback;
        std::string mTextualAddress;

        ossia::val_type mValueType{};
        ossia::access_mode mAccessMode{};
        ossia::bounding_mode mBoundingMode{};
        ossia::repetition_filter mRepetitionFilter{};

    public:
        ossia::value PreviousValue;
        generic_address(const ossia::net::node_base& node_base);

        ~generic_address();

        const ossia::net::node_base& getNode() const override;

        void pullValue() override;

        ossia::net::address_base & pushValue(const ossia::value&) override;
        ossia::net::address_base & pushValue() override;

        const ossia::value& getValue() const;
        ossia::value cloneValue(ossia::destination_index = {}) const override;
        address_base & setValue(const ossia::value&) override;

        ossia::val_type getValueType() const override;
        ossia::net::address_base & setValueType(ossia::val_type) override;

        ossia::access_mode getAccessMode() const override;
        ossia::net::address_base & setAccessMode(ossia::access_mode) override;

        const ossia::net::domain& getDomain() const override;
        ossia::net::address_base & setDomain(const ossia::net::domain&) override;

        ossia::bounding_mode getBoundingMode() const override;
        ossia::net::address_base & setBoundingMode(ossia::bounding_mode) override;

        ossia::repetition_filter getRepetitionFilter() const override;
        ossia::net::address_base & setRepetitionFilter(ossia::repetition_filter) override;

        void onFirstCallbackAdded() override;
        void onRemovingLastCallback() override;

        const std::string& getTextualAddress() const override
        { return mTextualAddress; }

    private:
};
}
}
