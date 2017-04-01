#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/base/device.hpp>

namespace ossia
{
struct execution_state
{
  std::vector<ossia::net::device_base*> globalState;
  std::unordered_map<destination_t, data_type> localState;

  bool in_local_scope(ossia::net::address_base& other) const
  {
    bool ok = (localState.find(destination_t{&other}) != localState.end());
    if(!ok)
    {
      // TODO check if there is any pattern matching the current destination
    }
    return ok;
  }
};
}
