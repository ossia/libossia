#include <ossia/editor/scenario/execution_log.hpp>
#if defined(OSSIA_EXECUTION_LOG)
namespace ossia
{

OSSIA_EXPORT execution_log g_exec_log;
on_destruct execution_log::init()
{
  buf.Clear();
  buf.Reserve(1e8);
  w.Reset(buf);

  w.StartArray();
  return on_destruct{[this] { commit(); }};
}

on_destruct execution_log::start_tick()
{
  w.StartObject();
  return on_destruct{[this] { w.EndObject(); }};
}

on_destruct execution_log::start_temporal()
{
  w.Key("Temporal");
  w.StartArray();

  return on_destruct{[this] { w.EndArray(); }};
}

on_destruct execution_log::interval_start_start_tick(const std::string& name)
{                             /*
                                 w.StartObject();
                                 w.Key("Name");
                                 w.String(name);
                            
                                 w.Key("IntervalStart");
                            
                                 w.StartObject();
                                 w.Key("Processes");
                                 w.StartArray();*/
  return on_destruct{[this] { /*
         w.EndArray();
         w.EndObject();

         w.EndObject();*/
  }};
}

on_destruct execution_log::interval_start_run_tick(
    const std::string& name, time_value old_date, time_value new_date, time_value offset)
{
  w.StartObject();
  w.Key("Name");
  w.String(name);

  w.Key("IntervalRun");

  w.StartObject();
  w.Key("OldDate");
  w.Int64(old_date.impl);
  w.Key("NewDate");
  w.Int64(new_date.impl);
  w.Key("Offset");
  w.Int64(offset.impl);
  w.Key("Processes");
  w.StartArray();
  return on_destruct{[this] {
    w.EndArray();
    w.EndObject();

    w.EndObject();
  }};
}

on_destruct execution_log::process_state(const std::string& name)
{
  w.StartObject();
  w.Key("Name");
  w.String(name);

  w.Key("Children");
  w.StartArray();
  return on_destruct{[this] {
    w.EndArray();
    w.EndObject();
  }};
}

on_destruct execution_log::start_dataflow()
{
  w.Key("Dataflow");
  w.StartObject();
  return on_destruct{[this] { w.EndObject(); }};
}

on_destruct execution_log::log_executed_nodes(
    const graph_t& g, const std::vector<graph_node*>& nodes)
{
  w.Key("Graph");
  {
    std::stringstream s;
    boost::write_graphviz(
        s, g,
        [&](auto& out, auto v) {
      const std::shared_ptr<ossia::graph_node>& n = g[v];
      if(n && !n->label().empty())
        out << fmt::format(
            "[label=\"{}\",color={}]", n->label(), n->executed() ? "green" : "red");
      else
        out << fmt::format("[color={}]", n->executed() ? "green" : "red");
        },
        [](auto&&...) {});

    w.String(s.str());
  }

  w.Key("Order");
  w.StartArray();
  std::string s;
  for(auto n : nodes)
  {
    w.String(n->label());
  }
  w.EndArray();

  return [] {};
}

on_destruct execution_log::start_commit()
{
  w.Key("Commit");
  w.StartObject();
  return on_destruct{[this] { w.EndObject(); }};
}

void execution_log::commit()
{
  w.EndArray();
  const char* str = buf.GetString();
  const auto sz = buf.GetSize();

  using namespace std;
  std::ofstream f;

  f.open("/tmp/score_execution_log.json");
  f.write(str, sz);
  f.close();
}

}
#endif
