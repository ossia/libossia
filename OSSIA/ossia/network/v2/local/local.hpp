#pragma once

#include "Protocol.hpp"
#include <vector>
#include <Misc/Util.h>
namespace impl
{
class OSSIA_EXPORT Local2 final :
        public OSSIA::v2::Protocol
{
    public:
        Local2();

        virtual ~Local2();

        bool pullAddressValue(OSSIA::v2::Address&) const override;
        bool pushAddressValue(const OSSIA::v2::Address&) const override;
        bool observeAddressValue(OSSIA::v2::Address&, bool) const override;
        bool updateChildren(OSSIA::v2::Node& node) const override;

        void exposeTo(std::unique_ptr<OSSIA::v2::Protocol> p)
        {
            mExposed.push_back(std::move(p));
        }

        void stopExposeTo(const OSSIA::v2::Protocol& p)
        {
            mExposed.erase(
                        OSSIA::remove_if(mExposed, [&] (const auto& ptr) { return ptr.get() == &p; }),
                        mExposed.end());
        }

        const auto& getExposedProtocols() const { return mExposed; }

    private:
        std::vector<std::unique_ptr<OSSIA::v2::Protocol>> mExposed;
};
}
