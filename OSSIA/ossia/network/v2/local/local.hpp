#pragma once

#include <ossia/network/v2/base/protocol.hpp>
#include <vector>
#include <ossia/detail/algorithms.hpp>
namespace impl
{
class OSSIA_EXPORT Local2 final :
        public OSSIA::v2::Protocol
{
    public:
        Local2();

        virtual ~Local2();

        bool pullAddressValue(OSSIA::v2::Address&) override;
        bool pushAddressValue(const OSSIA::v2::Address& addr) override;
        bool observeAddressValue(OSSIA::v2::Address&, bool) override;
        bool updateChildren(OSSIA::v2::Node& node) override;

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
