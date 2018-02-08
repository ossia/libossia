#pragma once
#include <tbb/flow_graph.h>
#include <ossia/dataflow/graph/graph_static.hpp>

namespace ossia
{
namespace tf = tbb::flow;
struct parallel_tc_graph final: public graph_static_base
{
public:
  using graph_static_base::graph_static_base;

  protected:
  struct node_exec
  {
      parallel_tc_graph& g;
      graph_node& node;

      void operator()(const tf::continue_msg&)
      {
        if(node.enabled())
        {
          assert(can_execute(node, *g.cur_state));
            g.exec_node(node, *g.cur_state);
        }
      }
  };

  struct node_exec_logger
  {
      parallel_tc_graph& g;
      spdlog::logger& logger;
      graph_node& node;

      void operator()(const tf::continue_msg&)
      {
        if(node.enabled())
        {
          assert(can_execute(node, *g.cur_state));
          if(!node.logged())
            g.exec_node(node, *g.cur_state);
          else
            g.exec_node(node, *g.cur_state, logger);
        }
      }
  };

  execution_state* cur_state{};
  void update_graph_tbb()
  {
    flow_graph = std::make_unique<tf::graph>();
    flow_nodes.clear();
    start_nodes.clear();
    nodes_with_incoming_edges.clear();

    for(auto n : m_nodes)
    {
      graph_node* node = n.first.get();
      if(logger)
        flow_nodes.insert({node, std::make_unique<tf::continue_node<tf::continue_msg>>(*flow_graph, node_exec_logger{*this, *logger, *node})});
      else
        flow_nodes.insert({node, std::make_unique<tf::continue_node<tf::continue_msg>>(*flow_graph, node_exec{*this, *node})});
    }

    for(auto n : m_nodes)
    {
      graph_node* n1 = n.first.get();
      for(auto m : m_nodes)
      {
        graph_node* n2 = m.first.get();
        if(n2 != n1)
        {
          if(boost::edge(n.second, m.second, m_sub_graph).second)
          {
            tf::make_edge(*flow_nodes[n2], *flow_nodes[n1]);
            nodes_with_incoming_edges.push_back(n1);
          }
        }
      }
    }

    for(auto n : m_all_nodes)
    {
      if(!ossia::contains(nodes_with_incoming_edges, n))
        start_nodes.push_back(n);
    }

    for (auto& node : start_nodes)
      std::cerr << "start_node: " << node->label() << " " <<  node << std::endl;

    start_node = std::make_unique<tf::broadcast_node<tf::continue_msg>>(*flow_graph);
    for(auto node : start_nodes)
    {
      tf::make_edge(*start_node, *flow_nodes[node]);
    }
  }
  std::unique_ptr<tf::broadcast_node< tf::continue_msg >> start_node;

  std::vector<graph_node*> nodes_with_incoming_edges;
  void state(execution_state& e) override
  {
    cur_state = &e;
    try
    {
      if(m_dirty)
      {
        update_graph_tclosure(e.allDevices);
        update_graph_tbb();
        m_dirty = false;
      }

      // Filter disabled nodes (through strict relationships).
      m_enabled_cache.reserve(m_nodes.size());

      for(auto node : m_all_nodes)
      {
        ossia::graph_node& ptr = *node;
        if(ptr.enabled()) {
          m_enabled_cache.insert(&ptr);
        }
        else
        {
          auto it = m_enabled_cache.find(&ptr);
          if(it != m_enabled_cache.end())
            m_enabled_cache.erase(it);
        }
      }

      disable_strict_nodes_rec(m_enabled_cache, m_disabled_cache);

      start_node->try_put(tf::continue_msg{});
      flow_graph->wait_for_all();

      finish_nodes(m_nodes);
    }
    catch (const boost::not_a_dag& e)
    {
      ossia::logger().error("Execution graph is not a DAG.");
      return;
    }
  }

  using transitive_closure_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, ossia::graph_node*, int64_t>;
  void compute_transitive_closure(const graph_t& sub_graph, transitive_closure_t& tclos)
  {
    tclos = transitive_closure_t{};
    ossia::transitive_closure(sub_graph, tclos, m_tcState);

#if defined(OSSIA_GRAPH_DEBUG)
    auto vertices = boost::vertices(sub_graph);
    for(auto i = vertices.first; i != vertices.second; i++)
    {
      tclos[*i] = sub_graph[*i].get();
      assert(tclos[*i]);
    }
    print_graph(tclos, std::cout);
#endif
  }

  template<typename DevicesT>
  void update_graph_tclosure(const DevicesT& devices)
  {
    m_sub_graph = m_graph;

    get_sorted_nodes(m_sub_graph);

    // m_active_nodes is in topo order

    // note: this is not enough.
    // eg consider
    //                 n1
    //              /       \  ..
    //  /a->n2->/b            /b->n3->/a
    // depending on the sort the connection may not happen, if n3 happens before n2
    //.. is it a problem ? we should just sort every "non-connected" node ?
    // What we do is : do the topo sort, and only add edges if they don't create cycles.
    // That is, if there is already path from n2 to n1 we don't add the edge.
    // The order is defined... by what ? maybe we have to run this every tick ? :(
    // If using the topo sort order (eg DFS) we can do it statically, else it's dynamically.

    // another case : [b -> c]   [a -> b]
    // if the first node occurs before the second there won't be any chaining, while we want to ensure that there
    // will be chainings.
    // So: for each pair: check if there is a path from one to the other.
    // Problem: [a -> b]  [b -> a] : which comes first ? one has to resolve the ambiguity manually.

    compute_transitive_closure(m_sub_graph, m_transitive_closure);

    for(std::size_t i = 0; i < m_all_nodes.size(); i++)
    {
      ossia::graph_node* n1 = m_all_nodes[i];
      for(std::size_t j = i + 1; j < m_all_nodes.size(); j++)
      {
        ossia::graph_node* n2 = m_all_nodes[j];

        auto source_vtx = m_nodes.find(n1)->second;
        auto sink_vtx = m_nodes.find(n2)->second;
        if(boost::edge(source_vtx, sink_vtx, m_transitive_closure).second)
          continue;
        if(boost::edge(sink_vtx, source_vtx, m_transitive_closure).second)
          continue;

        if(find_address_connection(*n1, *n2, devices))
        {
          auto src_it = m_nodes.find(n1);
          auto sink_it = m_nodes.find(n2);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          compute_transitive_closure(m_sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
          print_graph(transitive_closure, std::cout);
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
        else if(find_address_connection(*n2, *n1, devices))
        {
          auto src_it = m_nodes.find(n2);
          auto sink_it = m_nodes.find(n1);
          auto edge = ossia::make_edge(ossia::dependency_connection{}, ossia::outlet_ptr{}, ossia::inlet_ptr{},
                                       src_it->first, sink_it->first);
          boost::add_edge(sink_it->second, src_it->second, edge, m_sub_graph);
          compute_transitive_closure(m_sub_graph, m_transitive_closure);

#if defined(OSSIA_GRAPH_DEBUG)
          auto all_nodes_old = std::move(m_all_nodes);
          m_all_nodes.clear();
          get_sorted_nodes(sub_graph);
          m_all_nodes = std::move(all_nodes_old);
#endif
        }
      }
    }

    get_sorted_nodes(m_sub_graph);
  }
private:
  transitive_closure_t m_transitive_closure;
  ossia::TransitiveClosureState<graph_t, transitive_closure_t> m_tcState;
  std::unique_ptr<tbb::flow::graph> flow_graph;
  std::map<graph_node*, std::unique_ptr<tf::continue_node<tf::continue_msg>>> flow_nodes;
  std::vector<graph_node*> start_nodes;
};

}
