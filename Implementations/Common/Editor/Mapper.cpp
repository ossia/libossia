#include "Editor/Mapper_impl.h"
#include "Editor/Curve_impl.h"
#include <Editor/Value/Behavior.h>

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
shared_ptr<Mapper> Mapper::create(shared_ptr<Address> driverAddress,
                                  shared_ptr<Address> drivenAddress,
                                  const Value& drive)
{
  return make_shared<impl::JamomaMapper>(driverAddress, drivenAddress, drive);
}

Mapper::~Mapper() = default;
}

