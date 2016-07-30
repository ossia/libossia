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
class protocol;
}
}
namespace impl
{
class OSSIA_EXPORT BasicAddress final : public ossia::net::address
{
    private:
        const ossia::net::node& mNode;
        ossia::net::protocol& mProtocol;

        mutable std::mutex mValueMutex;
        ossia::value mValue;

        ossia::net::Domain mDomain;

        ossia::net::ValueCallback mCallback;
        std::string mTextualAddress;

        ossia::Type mValueType{};
        ossia::AccessMode mAccessMode{};
        ossia::BoundingMode mBoundingMode{};
        ossia::RepetitionFilter mRepetitionFilter{};

    public:
        ossia::value PreviousValue;
        BasicAddress(const ossia::net::node& node);

        ~BasicAddress();

        const ossia::net::node& getNode() const override;

        void pullValue() override;

        ossia::net::address & pushValue(const ossia::value&) override;
        ossia::net::address & pushValue() override;

        const ossia::value& getValue() const;
        ossia::value cloneValue(ossia::destination_index = {}) const override;
        address & setValue(const ossia::value&) override;

        ossia::Type getValueType() const override;
        ossia::net::address & setValueType(ossia::Type) override;

        ossia::AccessMode getAccessMode() const override;
        ossia::net::address & setAccessMode(ossia::AccessMode) override;

        const ossia::net::Domain& getDomain() const override;
        ossia::net::address & setDomain(const ossia::net::Domain&) override;

        ossia::BoundingMode getBoundingMode() const override;
        ossia::net::address & setBoundingMode(ossia::BoundingMode) override;

        ossia::RepetitionFilter getRepetitionFilter() const override;
        ossia::net::address & setRepetitionFilter(ossia::RepetitionFilter) override;

        void onFirstCallbackAdded() override;
        void onRemovingLastCallback() override;

        const std::string& getTextualAddress() const override
        { return mTextualAddress; }

    private:
};
}
