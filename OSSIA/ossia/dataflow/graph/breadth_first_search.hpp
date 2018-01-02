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
  Buffer& Q, BFSVisitor& vis, ColorMap color)
{
  using namespace boost;
  using GTraits = graph_traits<IncidenceGraph>;
  using ColorValue = typename property_traits<ColorMap>::value_type;
  typename GTraits::out_edge_iterator ei, ei_end;

  BOOST_CONCEPT_ASSERT(( IncidenceGraphConcept<IncidenceGraph> ));
  BOOST_CONCEPT_ASSERT(( BFSVisitorConcept<BFSVisitor, IncidenceGraph> ));
  BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<ColorMap, Vertex> ));

  put(color, source, two_bit_gray);
  vis.discover_vertex(source, g);
  Q.push(source);

  while (! Q.empty())
  {
    Vertex u = Q.top();
    Q.pop();
    for (std::tie(ei, ei_end) = out_edges(u, g); ei != ei_end; ++ei)
    {
      Vertex v = target(*ei, g);
      ColorValue v_color = get(color, v);
      if (v_color == two_bit_white)
      {
        put(color, v, two_bit_gray);
        if(vis.discover_vertex(v, g))
          return;
        Q.push(v);
      }
    }
    put(color, u, two_bit_black);
  }
}

template <class VertexListGraph, class Visitor>
void breadth_first_search_simple(
  const VertexListGraph& g,
  typename boost::graph_traits<VertexListGraph>::vertex_descriptor s,
  Visitor& vis)
{
  using namespace boost;

  typedef graph_traits<VertexListGraph> Traits;
  typedef typename Traits::vertex_descriptor Vertex;

  boost::queue<Vertex> Q;

  auto color = make_two_bit_color_map(
    num_vertices(g),
    choose_const_pmap(get_param(vis, vertex_index), g, vertex_index));

  typename boost::graph_traits<VertexListGraph>::vertex_iterator i, i_end;
  for (boost::tie(i, i_end) = vertices(g); i != i_end; ++i) {
    put(color, *i, two_bit_white);
  }
  breadth_first_visit_simple(g, s, Q, vis, color);
}
}
}
