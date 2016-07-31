#include <ossia/editor/mapper/detail/Mapper_impl.hpp>

namespace ossia
{
std::shared_ptr<mapper> mapper::create(
    ossia::net::address_base& driverAddress,
    ossia::net::address_base& drivenAddress,
    const ossia::value& drive)
{
  return std::make_shared<detail::mapper_impl>(driverAddress, drivenAddress, drive);
}

mapper::~mapper() = default;
}

