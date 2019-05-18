#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

namespace boost
{
template <typename IndexMap = identity_property_map>
struct two_bit_color_map_fast
{
  std::size_t n;
  IndexMap index;
  mutable std::vector<unsigned char> data;

  BOOST_STATIC_CONSTANT(
      int, bits_per_char = std::numeric_limits<unsigned char>::digits);
  BOOST_STATIC_CONSTANT(int, elements_per_char = bits_per_char / 2);
  typedef typename property_traits<IndexMap>::key_type key_type;
  typedef two_bit_color_type value_type;
  typedef void reference;
  typedef read_write_property_map_tag category;

  explicit two_bit_color_map_fast(
      std::size_t n, const IndexMap& index = IndexMap())
      : n(n)
      , index(index)
      , data((n + elements_per_char - 1) / elements_per_char)
  {
    // Fill to white
    std::fill(data.begin(), data.end(), 0);
  }

  void resize(std::size_t n)
  {
    this->n = n;
    data.resize((n + elements_per_char - 1) / elements_per_char);
    std::fill(data.begin(), data.end(), 0);
  }
};

template <typename IndexMap>
inline two_bit_color_type
get(const two_bit_color_map_fast<IndexMap>& pm,
    typename property_traits<IndexMap>::key_type key)
{
  BOOST_STATIC_CONSTANT(
      int,
      elements_per_char = two_bit_color_map_fast<IndexMap>::elements_per_char);
  typename property_traits<IndexMap>::value_type i = get(pm.index, key);
  BOOST_ASSERT((std::size_t)i < pm.n);

  std::size_t byte_num = i / elements_per_char;
  std::size_t bit_position = ((i % elements_per_char) * 2);

  return two_bit_color_type((pm.data[byte_num] >> bit_position) & 3);
}

template <typename IndexMap>
inline void
put(const two_bit_color_map_fast<IndexMap>& pm,
    typename property_traits<IndexMap>::key_type key, two_bit_color_type value)
{
  BOOST_STATIC_CONSTANT(
      int,
      elements_per_char = two_bit_color_map_fast<IndexMap>::elements_per_char);
  typename property_traits<IndexMap>::value_type i = get(pm.index, key);
  BOOST_ASSERT((std::size_t)i < pm.n);
  BOOST_ASSERT(value >= 0 && value < 4);

  std::size_t byte_num = i / elements_per_char;
  std::size_t bit_position = ((i % elements_per_char) * 2);

  pm.data[byte_num]
      = (unsigned char)((pm.data[byte_num] & ~(3 << bit_position)) | (value << bit_position));
}

template <typename IndexMap>
inline two_bit_color_map_fast<IndexMap>
make_two_bit_color_map_fast(std::size_t n, const IndexMap& index_map)
{
  return two_bit_color_map_fast<IndexMap>(n, index_map);
}

} // end namespace boost

namespace ossia
{
// Based on the boost BFS code, but allows to early abort and does not call
// twenty different functions
namespace bfs
{
template <
    class IncidenceGraph, class Vertex, class Buffer, class BFSVisitor,
    class ColorMap>
void breadth_first_visit_simple(
    const IncidenceGraph& g, Vertex source, Buffer& Q, BFSVisitor& vis,
    ColorMap& color)
{
  using namespace boost;
  using GTraits = graph_traits<IncidenceGraph>;
  typename GTraits::out_edge_iterator ei, ei_end;

  BOOST_CONCEPT_ASSERT((IncidenceGraphConcept<IncidenceGraph>));

  put(color, source, two_bit_gray);
  vis.discover_vertex(source, g);
  Q.push_back(source);

  while (!Q.empty())
  {
    Vertex u = Q.front();
    Q.pop_front();
    for (std::tie(ei, ei_end) = out_edges(u, g); ei != ei_end; ++ei)
    {
      Vertex v = target(*ei, g);
      auto v_color = get(color, v);
      if (v_color == two_bit_white)
      {
        put(color, v, two_bit_gray);
        if (vis.discover_vertex(v, g))
          return;
        Q.push_back(v);
      }
    }
    put(color, source, two_bit_black);
  }
}

template <class VertexListGraph, class Visitor, class Queue, class ColorMap>
void breadth_first_search_simple(
    const VertexListGraph& g,
    typename boost::graph_traits<VertexListGraph>::vertex_descriptor s,
    Visitor& vis, Queue& Q, ColorMap& color)
{
  using namespace boost;
  // auto color = make_two_bit_color_map_fast(boost::num_vertices(g),
  // choose_const_pmap(get_param(vis, vertex_index), g, vertex_index));
  // make_two_bit_color_map_fast(boost::num_vertices(g), vertex_index);
  typename boost::graph_traits<VertexListGraph>::vertex_iterator i, i_end;
  for (boost::tie(i, i_end) = vertices(g); i != i_end; ++i)
    put(color, *i, two_bit_white);

  breadth_first_visit_simple(g, s, Q, vis, color);
}
}
}
