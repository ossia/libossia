#pragma once
#include <ossia/detail/config.hpp>

#if 0 //! defined(NDEBUG)
#define OSSIA_EXECUTION_LOG 1
#include <ossia/dataflow/graph/graph_utils.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/detail/json.hpp>

#include <rapidjson/prettywriter.h>

#include <functional>

namespace ossia
{
struct on_destruct {
  std::function<void()> dest;
  template<typename F>
  inline on_destruct(F t): dest{std::move(t)} { }
  inline on_destruct(on_destruct&& t): dest{std::move(t.dest)} { t.dest = [] {}; }
  inline on_destruct& operator=(on_destruct&& t) { dest = std::move(t.dest); t.dest = [] {}; return *this; }

  inline on_destruct() = delete;
  inline on_destruct(const on_destruct& t) = delete;
  inline on_destruct& operator=(const on_destruct& t) = delete;

  inline ~on_destruct() { dest(); }
};
struct OSSIA_EXPORT execution_log
{
  rapidjson::StringBuffer buf;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> w{buf};


  [[nodiscard]]
  on_destruct init();

  [[nodiscard]]
  on_destruct start_tick();

  [[nodiscard]]
  on_destruct start_temporal();

  [[nodiscard]]
  on_destruct interval_start_start_tick(const std::string& name);

  [[nodiscard]]
  on_destruct interval_start_run_tick(const std::string& name, time_value old_date, time_value new_date, time_value offset);

  [[nodiscard]]
  on_destruct process_state(const std::string& name);

  [[nodiscard]]
  on_destruct start_dataflow();

  [[nodiscard]]
  on_destruct log_executed_nodes(const ossia::graph_t&, const std::vector<graph_node*>& );

  [[nodiscard]]
  on_destruct start_commit();


  void commit();
};

extern OSSIA_EXPORT execution_log g_exec_log;
}

#endif
