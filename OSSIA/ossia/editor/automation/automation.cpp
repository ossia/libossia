#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/automation/detail/Automation_impl.hpp>

namespace OSSIA
{
std::shared_ptr<Automation> Automation::create(
        std::shared_ptr<Address> address,
        const Value& drive)
{
    return std::make_shared<impl::JamomaAutomation>(address, drive);
}

Automation::~Automation() = default;
}


