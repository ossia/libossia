#pragma once
#include <boost/graph/breadth_first_search.hpp>

namespace ossia
{
// Based on the boost BFS code, but allows to early abort and does not call twenty different functions
namespace bfs
{
template <class IncidenceGraph, class Vertex, class Buffer, class BFSVisitor, class ColorMap>
void breadth_first_visit_simple(
  const IncidenceGraph& g, Vertex source,
  Buffer& Q, BFSVisitor& vis, ColorMap& color)
{
  using namespace boost;
  using GTraits = graph_traits<IncidenceGraph>;
  typename GTraits::out_edge_iterator ei, ei_end;

  BOOST_CONCEPT_ASSERT(( IncidenceGraphConcept<IncidenceGraph> ));
  BOOST_CONCEPT_ASSERT(( BFSVisitorConcept<BFSVisitor, IncidenceGraph> ));

  color[source] = two_bit_gray;
  vis.discover_vertex(source, g);
  Q.push_back(source);

  while (! Q.empty())
  {
    Vertex u = Q.front();
    Q.pop_front();
    for (std::tie(ei, ei_end) = out_edges(u, g); ei != ei_end; ++ei)
    {
      Vertex v = target(*ei, g);
      auto& v_color = color.find(v)->second;
      if (v_color == two_bit_white)
      {
        v_color = two_bit_gray;
        if(vis.discover_vertex(v, g))
          return;
        Q.push_back(v);
      }
    }
    color[u] = two_bit_black;
  }
}

template <class VertexListGraph, class Visitor, class Queue, class ColorMap>
void breadth_first_search_simple(
  const VertexListGraph& g,
  typename boost::graph_traits<VertexListGraph>::vertex_descriptor s,
  Visitor& vis,
  Queue& Q,
  ColorMap& color)
{
  using namespace boost;
  typename boost::graph_traits<VertexListGraph>::vertex_iterator i, i_end;
  for (boost::tie(i, i_end) = vertices(g); i != i_end; ++i)
    color[*i] = two_bit_white;

  breadth_first_visit_simple(g, s, Q, vis, color);
}

}
}
