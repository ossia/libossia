#pragma once

#include <ossia/network/base/protocol.hpp>
#include <vector>
#include <ossia/detail/algorithms.hpp>
namespace impl
{
class OSSIA_EXPORT Local2 final :
        public ossia::net::protocol
{
    public:
        Local2();

        virtual ~Local2();

        bool pull(ossia::net::address&) override;
        bool push(const ossia::net::address& addr) override;
        bool observe(ossia::net::address&, bool) override;
        bool update(ossia::net::node& node) override;

        void exposeTo(std::unique_ptr<ossia::net::protocol> p)
        {
            mExposed.push_back(std::move(p));
        }

        void stopExposeTo(const ossia::net::protocol& p)
        {
            mExposed.erase(
                        ossia::remove_if(mExposed, [&] (const auto& ptr) { return ptr.get() == &p; }),
                        mExposed.end());
        }

        const auto& getExposedProtocols() const { return mExposed; }

    private:
        std::vector<std::unique_ptr<ossia::net::protocol>> mExposed;
};
}
