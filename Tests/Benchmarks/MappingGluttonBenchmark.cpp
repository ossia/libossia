#include <ossia/detail/any.hpp>
#include <random>
#include <ossia/detail/pod_vector.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <flat_hash_map.hpp>

#include <ossia/dataflow/nodes/automation.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <valgrind/callgrind.h>
#include <random>
#define private public
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>

#define immediate_strict_connection immediate_glutton_connection
#include "MappingStrictBenchmark.cpp"
