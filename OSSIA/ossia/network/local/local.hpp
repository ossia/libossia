#pragma once

#include <ossia/network/base/protocol.hpp>
#include <vector>
#include <ossia/detail/algorithms.hpp>
namespace impl
{
class OSSIA_EXPORT Local2 final :
        public OSSIA::net::Protocol
{
    public:
        Local2();

        virtual ~Local2();

        bool pull(OSSIA::net::Address&) override;
        bool push(const OSSIA::net::Address& addr) override;
        bool observe(OSSIA::net::Address&, bool) override;
        bool update(OSSIA::net::Node& node) override;

        void exposeTo(std::unique_ptr<OSSIA::net::Protocol> p)
        {
            mExposed.push_back(std::move(p));
        }

        void stopExposeTo(const OSSIA::net::Protocol& p)
        {
            mExposed.erase(
                        OSSIA::remove_if(mExposed, [&] (const auto& ptr) { return ptr.get() == &p; }),
                        mExposed.end());
        }

        const auto& getExposedProtocols() const { return mExposed; }

    private:
        std::vector<std::unique_ptr<OSSIA::net::Protocol>> mExposed;
};
}
