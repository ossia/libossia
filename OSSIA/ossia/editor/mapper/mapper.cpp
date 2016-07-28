#include <ossia/editor/mapper/detail/Mapper_impl.hpp>

namespace OSSIA
{
std::shared_ptr<Mapper> Mapper::create(
    OSSIA::net::Address& driverAddress,
    OSSIA::net::Address& drivenAddress,
    const OSSIA::Value& drive)
{
  return std::make_shared<impl::JamomaMapper>(driverAddress, drivenAddress, drive);
}

Mapper::~Mapper() = default;
}

