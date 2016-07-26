#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/automation/detail/Automation_impl.hpp>

namespace OSSIA
{
  shared_ptr<Automation> Automation::create(shared_ptr<Address> address,
                                            const Value& drive)
  {
    return make_shared<impl::JamomaAutomation>(address, drive);
  }

  Automation::~Automation()
  {
  }
}


