#pragma once
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia
{

OSSIA_EXPORT
void map_value(
    ossia::value& source, const ossia::destination_index& idx,
    const ossia::domain& source_domain, const ossia::domain& target_domain);

}
