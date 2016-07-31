#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/automation/detail/Automation_impl.hpp>

namespace ossia
{
std::shared_ptr<automation> automation::create(
        ossia::net::address_base& address,
        const value& drive)
{
    return std::make_shared<detail::automation_impl>(address, drive);
}

automation::~automation() = default;
}


