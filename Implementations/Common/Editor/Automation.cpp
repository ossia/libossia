#include <Editor/Automation.h>
#include <Editor/Automation_impl.h>

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


