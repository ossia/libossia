#pragma once
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
namespace ossia
{

using scenario_graph_vertex = time_sync*;
using scenario_graph_edge = time_interval*;

using scenario_graph = boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::undirectedS,
  scenario_graph_vertex,
  scenario_graph_edge>;

struct sgraph
{
    scenario& scenar;
    scenario_graph m_graph;
    tsl::hopscotch_map<
        const time_sync*,
        scenario_graph::vertex_descriptor> m_vertices;
    tsl::hopscotch_map<
        const time_interval*,
        scenario_graph::edge_descriptor> m_edges;

    sgraph(scenario& sc):
      scenar{sc}
    {
      for(auto& tn : scenar.get_time_syncs())
      {
        m_vertices[tn.get()] = boost::add_vertex(tn.get(), m_graph);
      }

      for(auto& cst : scenar.get_time_intervals())
      {
        m_edges[cst.get()] = boost::add_edge(
              m_vertices[&cst->get_start_event().get_time_sync()],
              m_vertices[&cst->get_end_event().get_time_sync()],
              cst.get(),
              m_graph).first;
      }
    }

    small_sync_vec get_roots() const
    {
      small_sync_vec res;
      std::vector<int> component(boost::num_vertices(m_graph));
      boost::connected_components(m_graph, &component[0]);

      int root_comp = component[m_vertices.at(scenar.get_start_time_sync().get())];

      for(auto& tn : scenar.get_time_syncs())
      {
        if(scenar.is_root_sync(*tn) && component[m_vertices.at(tn.get())] != root_comp)
        {
          res.push_back(tn.get());
        }
      }

      return res;
    }

    auto components() const
    {
      std::vector<int> component(boost::num_vertices(m_graph));
      boost::connected_components(m_graph, &component[0]);
      return component;
    }

    void reset_component(ossia::time_sync& sync) const
    {
      return reset_component(components(), sync);
    }

    ossia::small_vector<ossia::time_sync*, 2> sibling_roots(
        const std::vector<int>& component,
        const ossia::time_sync& sync) const
    {
      ossia::small_vector<ossia::time_sync*, 2> res;
      auto comp = component[m_vertices.at(&sync)];

      for(const auto& s : scenar.get_time_syncs())
      {
        if(component[m_vertices.at(s.get())] == comp && scenar.is_root_sync(*s))
        {
          res.push_back(s.get());
        }
      }

      return res;
    }

    void reset_component(const std::vector<int>& component, ossia::time_sync& sync) const
    {
      std::vector<std::shared_ptr<ossia::time_sync>> to_disable_sync;
      std::vector<std::shared_ptr<ossia::time_interval>> to_disable_itv;
      auto comp = component[m_vertices.at(&sync)];
      for(auto s : scenar.get_time_syncs())
      {
        auto this_comp = component[m_vertices.at(s.get())];
        if(this_comp == comp)
        {
          to_disable_sync.push_back(s);
          for(auto& ev : s->get_time_events())
          {
            for(auto& cst : ev->previous_time_intervals())
            {
              to_disable_itv.push_back(cst);
            }
            for(auto& cst : ev->next_time_intervals())
            {
              to_disable_itv.push_back(cst);
            }
          }
        }
      }

      scenar.reset_subgraph(to_disable_sync, to_disable_itv, sync);
    }

};

}
