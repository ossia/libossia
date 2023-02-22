#include <ossia/dataflow/nodes/automation.hpp>
#include <ossia/detail/any.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include <boost/graph/adjacency_list.hpp>

#include <valgrind/callgrind.h>

#include <ossia/detail/hash_map.hpp>

#include <random>
#define private public
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/editor/scenario/scenario.hpp>

#define immediate_strict_connection immediate_glutton_connection
#include "MappingStrictBenchmark.cpp"
