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

namespace OSSIA
{
namespace net
{
class Protocol;
}
}
namespace impl
{
class OSSIA_EXPORT BasicAddress final : public OSSIA::net::Address
{
    private:
        const OSSIA::net::Node& mNode;
        OSSIA::net::Protocol& mProtocol;

        mutable std::mutex mValueMutex;
        OSSIA::Value mValue;

        OSSIA::net::Domain mDomain;

        OSSIA::net::ValueCallback mCallback;
        std::string mTextualAddress;

        OSSIA::Type mValueType{};
        OSSIA::AccessMode mAccessMode{};
        OSSIA::BoundingMode mBoundingMode{};
        OSSIA::RepetitionFilter mRepetitionFilter{};

    public:
        OSSIA::Value PreviousValue;
        BasicAddress(const OSSIA::net::Node& node);

        ~BasicAddress();

        const OSSIA::net::Node& getNode() const override;

        void pullValue() override;

        OSSIA::net::Address & pushValue(const OSSIA::Value&) override;
        OSSIA::net::Address & pushValue() override;

        const OSSIA::Value& getValue() const;
        OSSIA::Value cloneValue(OSSIA::DestinationIndex = {}) const override;
        Address & setValue(const OSSIA::Value&) override;

        OSSIA::Type getValueType() const override;
        OSSIA::net::Address & setValueType(OSSIA::Type) override;

        OSSIA::AccessMode getAccessMode() const override;
        OSSIA::net::Address & setAccessMode(OSSIA::AccessMode) override;

        const OSSIA::net::Domain& getDomain() const override;
        OSSIA::net::Address & setDomain(const OSSIA::net::Domain&) override;

        OSSIA::BoundingMode getBoundingMode() const override;
        OSSIA::net::Address & setBoundingMode(OSSIA::BoundingMode) override;

        OSSIA::RepetitionFilter getRepetitionFilter() const override;
        OSSIA::net::Address & setRepetitionFilter(OSSIA::RepetitionFilter) override;

        void onFirstCallbackAdded() override;
        void onRemovingLastCallback() override;

        const std::string& getTextualAddress() const override
        { return mTextualAddress; }

    private:
};
}
