#pragma once

#include <ossia/network/base/protocol.hpp>
#include <vector>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
namespace net
{
class OSSIA_EXPORT local_protocol final :
        public ossia::net::protocol_base
{
    public:
        local_protocol();

        virtual ~local_protocol();

        bool pull(ossia::net::address_base&) override;
        bool push(const ossia::net::address_base& addr) override;
        bool observe(ossia::net::address_base&, bool) override;
        bool update(ossia::net::node_base& node_base) override;

        void exposeTo(std::unique_ptr<ossia::net::protocol_base> p)
        {
            mExposed.push_back(std::move(p));
        }

        void stopExposeTo(const ossia::net::protocol_base& p)
        {
            mExposed.erase(
                        ossia::remove_if(mExposed, [&] (const auto& ptr) { return ptr.get() == &p; }),
                        mExposed.end());
        }

        const auto& getExposedProtocols() const { return mExposed; }

    private:
        std::vector<std::unique_ptr<ossia::net::protocol_base>> mExposed;
};
}
}
