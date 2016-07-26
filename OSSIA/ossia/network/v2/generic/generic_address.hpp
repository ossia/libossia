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
#include "Address.hpp"
#include "BasicDevice.hpp"
#include <Editor/Value/Value.h>
#include <Misc/CallbackContainer.h>

#include <thread>
#include <mutex>

namespace OSSIA
{
namespace v2
{
class Protocol;
}
}
namespace impl
{
class BasicAddress final : public OSSIA::v2::Address
{
    private:
        const OSSIA::v2::Node& mNode;
        OSSIA::v2::Protocol& mProtocol;

        mutable std::mutex mValueMutex;
        OSSIA::Value mValue;

        OSSIA::v2::Domain mDomain;

        OSSIA::v2::ValueCallback mCallback;
        std::string mTextualAddress;

        OSSIA::Type mValueType{};
        OSSIA::AccessMode mAccessMode{};
        OSSIA::BoundingMode mBoundingMode{};
        bool mRepetitionFilter{};

    public:
        OSSIA::Value PreviousValue;
        BasicAddress(const OSSIA::v2::Node& node);

        ~BasicAddress();

        const OSSIA::v2::Node& getNode() const override;

        void pullValue() override;

        OSSIA::v2::Address & pushValue(const OSSIA::Value&) override;
        OSSIA::v2::Address & pushValue() override;

        const OSSIA::Value& getValue() const;
        OSSIA::Value cloneValue(DestinationIndex = {}) const override;
        Address & setValue(const OSSIA::Value&) override;

        OSSIA::Type getValueType() const override;
        OSSIA::v2::Address & setValueType(OSSIA::Type) override;

        OSSIA::AccessMode getAccessMode() const override;
        OSSIA::v2::Address & setAccessMode(OSSIA::AccessMode) override;

        const OSSIA::v2::Domain& getDomain() const override;
        OSSIA::v2::Address & setDomain(const OSSIA::v2::Domain&) override;

        OSSIA::BoundingMode getBoundingMode() const override;
        OSSIA::v2::Address & setBoundingMode(OSSIA::BoundingMode) override;

        bool getRepetitionFilter() const override;
        OSSIA::v2::Address & setRepetitionFilter(bool) override;

        OSSIA::v2::Address::iterator addCallback(OSSIA::v2::ValueCallback) override;
        void removeCallback(OSSIA::v2::Address::iterator) override;

        const std::string& getTextualAddress() const override
        { return mTextualAddress; }

    private:
};
}
