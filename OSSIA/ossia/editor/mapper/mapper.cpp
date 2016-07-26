#include <ossia/editor/mapper/detail/Mapper_impl.hpp>
#include <ossia/editor/curve/detail/Curve_impl.hpp>
#include <ossia/editor/value/behavior.hpp>

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
std::shared_ptr<Mapper> Mapper::create(std::shared_ptr<Address> driverAddress,
                                  std::shared_ptr<Address> drivenAddress,
                                  const Value& drive)
{
  return std::make_shared<impl::JamomaMapper>(driverAddress, drivenAddress, drive);
}

Mapper::~Mapper() = default;
}

