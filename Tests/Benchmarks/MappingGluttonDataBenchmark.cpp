
#include <ossia/dataflow/graph/graph_static.hpp>

#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <valgrind/callgrind.h>
#include <random>
#include "../Editor/TestUtils.hpp"
#define immediate_strict_connection immediate_glutton_connection
#include "MappingStrictDataBenchmark.cpp"
