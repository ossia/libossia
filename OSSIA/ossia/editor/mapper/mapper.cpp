#include <ossia/editor/mapper/detail/Mapper_impl.hpp>

namespace ossia
{
std::shared_ptr<mapper> mapper::create(
    ossia::net::address& driverAddress,
    ossia::net::address& drivenAddress,
    const ossia::value& drive)
{
  return std::make_shared<impl::JamomaMapper>(driverAddress, drivenAddress, drive);
}

mapper::~mapper() = default;
}

