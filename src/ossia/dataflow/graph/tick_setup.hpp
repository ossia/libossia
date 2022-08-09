#pragma once
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/transport.hpp>

namespace ossia
{
class scenario;
OSSIA_EXPORT
std::shared_ptr<bench_map> bench_ptr();

OSSIA_EXPORT
smallfun::function<void(unsigned long, double), 128> make_tick(
    const tick_setup_options& settings, ossia::execution_state& st,
    ossia::graph_interface& e, ossia::time_interval& itv, ossia::scenario& root,
    ossia::transport_info_fun transport);
}
