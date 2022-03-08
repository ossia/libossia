#include "node_functions.hpp"

#include <ossia/detail/small_vector.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/path.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <ossia/detail/algorithms.hpp>
#include <bitset>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <utility>
#include <vector>
#include <regex>

#include <rapidfuzz/fuzz.hpp>

namespace ossia
{
namespace net
{
namespace
{
static node_base* find_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto child = node.find_child(address.substr(0, first_slash_index));
    if (child)
    {
      // There are still nodes since we found a slash
      return find_node_rec(*child, address.substr(first_slash_index + 1));
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    // One of the child may be the researched node.
    return node.find_child(address);
  }
}

static node_base& find_or_create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if (cld)
    {
      // There are still nodes since we found a slash
      return find_or_create_node_rec(
          *cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = *node.create_child(std::string(cur));

      // Recurse on it
      return find_or_create_node_rec(
          child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // One of the child may be the researched node.
    auto n = node.find_child(address);
    if (n)
    {
      return *n;
    }
    else
    {
      // Create and return the node
      return *node.create_child(std::string(address));
    }
  }
}

static node_base& create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if (cld)
    {
      // There are still nodes since we found a slash
      return create_node_rec(*cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto child = node.create_child(std::string(cur));
      if (!child)
        throw std::runtime_error{"create_node_rec: cannot create the node"};

      // Recurse on it
      return create_node_rec(*child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // Create and return the node
    auto child = node.create_child(std::string(address));
    if (!child)
      throw std::runtime_error{"create_node_rec: cannot create the node"};
    return *child;
  }
}

//! Note : here we modify the string_view only.
//! The original address remains unchanged.
static ossia::string_view sanitize_address(ossia::string_view address)
{
  while (boost::algorithm::starts_with(address, "/"))
    address.remove_prefix(1);
  while (boost::algorithm::ends_with(address, "/"))
    address.remove_suffix(1);
  return address;
}
}

node_base* find_node(node_base& dev, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return &dev;

  // address now looks like a/b/c
  return find_node_rec(dev, address);
}

node_base& find_or_create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return node;

  // address now looks like a/b/c
  return find_or_create_node_rec(node, address);
}

node_base& create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    address = "node";

  // address now looks like a/b/c
  return create_node_rec(node, address);
}

node_base*
find_or_create_node(node_base& dev, string_view parameter_base, bool reading)
{
  if (reading)
  {
    return ossia::net::find_node(dev, parameter_base);
  }
  else
  {
    return &ossia::net::create_node(dev, parameter_base);
  }
}

std::vector<node_base*> find_nodes(node_base& root, string_view pattern)
{
  if (!ossia::traversal::is_pattern(pattern))
  {
    auto node = ossia::net::find_node(root, pattern);
    if (node)
      return {node};
    else
      return {};
  }
  else if (auto path = traversal::make_path(pattern))
  {
    std::vector<node_base*> nodes{&root};
    traversal::apply(*path, nodes);
    return nodes;
  }
  else
  {
    auto node = ossia::net::find_node(root, pattern);
    if (node)
      return {node};
    else
      return {};
  }
}

// The following code, up to and including `List expand(const Range & range)`
// originates from Rosetta Code, under GNU FDL:
// https://rosettacode.org/wiki/Brace_expansion#C.2B.2B
namespace detail
{

template <typename ForwardIterator>
class tokenizer
{

  ForwardIterator _tbegin, _tend, _end;

public:
  tokenizer(ForwardIterator begin, ForwardIterator end)
      : _tbegin(begin), _tend(begin), _end(end)
  {
  }

  template <typename Lambda>
  bool next(Lambda istoken)
  {
    if (_tbegin == _end)
    {
      return false;
    }
    _tbegin = _tend;
    for (; _tend != _end && !istoken(*_tend); ++_tend)
    {
      if (*_tend == '\\' && std::next(_tend) != _end)
      {
        ++_tend;
      }
    }
    if (_tend == _tbegin && _tend != _end)
    {
      _tend++;
    }
    return _tbegin != _end;
  }

  ForwardIterator begin() const
  {
    return _tbegin;
  }
  ForwardIterator end() const
  {
    return _tend;
  }
  bool operator==(char c)
  {
    return *_tbegin == c;
  }
};

template <typename List>
void append_all(List& lista, const List& listb)
{
  if (listb.size() == 1)
  {
    for (auto& a : lista)
    {
      a += listb.back();
    }
  }
  else
  {
    List tmp;
    for (auto& a : lista)
    {
      for (auto& b : listb)
      {
        tmp.push_back(a + b);
      }
    }
    lista = std::move(tmp);
  }
}

template <typename String, typename List, typename Tokenizer>
List expand(Tokenizer& token)
{

  std::vector<List> alts{{String()}};

  while (token.next([](char c) { return c == '{' || c == ',' || c == '}'; }))
  {

    if (token == '{')
    {
      append_all(alts.back(), expand<String, List>(token));
    }
    else if (token == ',')
    {
      alts.push_back({String()});
    }
    else if (token == '}')
    {
      if (alts.size() == 1)
      {
        for (auto& a : alts.back())
        {
          a = '{' + a + '}';
        }
        return alts.back();
      }
      else
      {
        for (std::size_t i = 1; i < alts.size(); i++)
        {
          alts.front().insert(
              alts.front().end(), std::make_move_iterator(std::begin(alts[i])),
              std::make_move_iterator(std::end(alts[i])));
        }
        return std::move(alts.front());
      }
    }
    else
    {
      for (auto& a : alts.back())
      {
        a.append(token.begin(), token.end());
      }
    }
  }

  List result{String{'{'}};
  append_all(result, alts.front());
  for (std::size_t i = 1; i < alts.size(); i++)
  {
    for (auto& a : result)
    {
      a += ',';
    }
    append_all(result, alts[i]);
  }
  return result;
}

} // namespace detail

template <
    typename ForwardIterator,
    typename String = std::basic_string<
        typename std::iterator_traits<ForwardIterator>::value_type>,
    typename List = std::vector<String>>
List expand(ForwardIterator begin, ForwardIterator end)
{
  detail::tokenizer<ForwardIterator> token(begin, end);
  List list{String()};
  while (token.next([](char c) { return c == '{'; }))
  {
    if (token == '{')
    {
      detail::append_all(list, detail::expand<String, List>(token));
    }
    else
    {
      for (auto& a : list)
      {
        a.append(token.begin(), token.end());
      }
    }
  }
  return list;
}

template <
    typename Range,
    typename String = std::basic_string<typename Range::value_type>,
    typename List = std::vector<String>>
List expand(const Range& range)
{
  using Iterator = typename Range::const_iterator;
  return expand<Iterator, String, List>(std::begin(range), std::end(range));
}

void expand_ranges(std::string& str)
{
  {
    static const std::regex reg{
        R"_(\{(-?[0-9]+)\.\.(-?[0-9]+)\.\.(-?[0-9]+)\})_"};

    struct rx_triple
    {
      std::size_t start{}, length{};
      int64_t first{}, last{}, increment{};
    };
    ossia::small_vector<rx_triple, 4> positions;

    std::regex_iterator<std::string::iterator> rit(
        str.begin(), str.end(), reg);
    std::regex_iterator<std::string::iterator> rend;

    for (auto it = rit; it != rend; ++it)
    {
      int fst = std::stoi(it->str(1));
      int lst = std::stoi(it->str(2));
      int inc = std::stoi(it->str(3));
      if (inc != 0)
      {
        positions.push_back(
            rx_triple{(std::size_t)it->position(), (std::size_t)it->length(),
                      std::min(fst, lst), std::max(fst, lst), inc});
      }
    }

    for (auto it = positions.rbegin(); it != positions.rend(); ++it)
    {
      std::string rep{"{"};
      rep.reserve(3 * std::abs((it->last - it->first) / it->increment));
      if (it->increment > 0)
      {
        for (int64_t v = it->first; v <= it->last; v += it->increment)
        {
          rep += std::to_string(v);
          rep += ',';
        }
      }
      else
      {
        for (int64_t v = it->last; v <= it->first; v += it->increment)
        {
          rep += std::to_string(v);
          rep += ',';
        }
      }

      if (rep.back() == ',')
      {
        rep.back() = '}';
        str.replace(it->start, it->length, rep);
      }
    }
  }

  {
    static const std::regex reg{R"_(\{(-?[0-9]+)\.\.(-?[0-9]+)\})_"};

    struct rx_double
    {
      std::size_t start{}, length{};
      int64_t first{}, last{};
    };
    ossia::small_vector<rx_double, 4> positions;

    std::regex_iterator<std::string::iterator> rit(
        str.begin(), str.end(), reg);
    std::regex_iterator<std::string::iterator> rend;

    for (auto it = rit; it != rend; ++it)
    {
      int fst = std::stoi(it->str(1));
      int lst = std::stoi(it->str(2));
      positions.push_back(rx_double{(std::size_t)it->position(),
                                    (std::size_t)it->length(),
                                    std::min(fst, lst), std::max(fst, lst)});
    }

    for (auto it = positions.rbegin(); it != positions.rend(); ++it)
    {
      if(it->last == it->first)
      {
        str.replace(it->start, it->length, std::to_string(it->first));
      }
      else
      {
        std::string rep;
        rep.reserve(3 * std::abs((it->last - it->first)) + 2);
        rep.push_back('{');
        for (int64_t v = it->first; v <= it->last; v++)
        {
          rep += std::to_string(v);
          rep += ',';
        }

        if (rep.back() == ',')
        {
          rep.back() = '}';
          str.replace(it->start, it->length, rep);
        }
      }
    }
  }
}

std::string canonicalize_str(std::string str)
{
  {
    // 1. find all [a-z0-9XYZ]
    static const std::regex rx_class{"\\[[a-zA-Z0-9-]+\\]"};

    std::replace(str.begin(), str.end(), '|', ',');
    struct rx_pos
    {
      std::size_t start{}, length{};
      std::bitset<128> chars;
    };
    ossia::small_vector<rx_pos, 4> positions;

    std::regex_iterator<std::string::iterator> rit(
        str.begin(), str.end(), rx_class);
    std::regex_iterator<std::string::iterator> rend;

    for (auto it = rit; it != rend; ++it)
    {
      auto theStr = it->str();
      theStr = theStr.substr(1, theStr.size() - 2);
      std::bitset<128> bits;

      for (int i = 0, N = theStr.size(); i < N;)
      {
        if ((N - i) > 2)
        {
          if (theStr[i + 1] == '-' && (int)theStr[i + 2] > (int)theStr[i])
          {
            for (int ch = (int)theStr[i]; ch <= (int)theStr[i + 2]; ++ch)
              bits[ch] = true;
            i += 2;
          }
          else
          {
            bits[(int)theStr[i]] = true;
            i++;
          }
        }
        else
        {
          bits[(int)theStr[i]] = true;
          i++;
        }
      }
      positions.push_back(rx_pos{(std::size_t)it->position(),
                                 (std::size_t)it->length(), bits});
    }

    for (auto it = positions.rbegin(); it != positions.rend(); ++it)
    {
      std::string rep{"{"};
      for (std::size_t i = 0; i < it->chars.size(); ++i)
      {
        if (it->chars[i])
        {
          rep += (char)i;
          rep += ',';
        }
      }

      if (rep.back() == ',')
      {
        rep.back() = '}';
        str.replace(it->start, it->length, rep);
      }
    }
  }

  expand_ranges(str);
  return str;
}

std::vector<node_base*> create_nodes(node_base& dev, string_view pattern)
{
  std::vector<node_base*> v;

  if (is_brace_expansion(pattern))
  {
    // 1. Replace all [ ] with { } form
    auto str = canonicalize_str(std::string(pattern));

    // 2. Expand
    auto expanded = expand(str);

    // 3. Create nodes
    v.reserve(expanded.size());
    for (const auto& addr : expanded)
    {
      v.push_back(&ossia::net::create_node(dev, addr));
    }
  }
  else
  {
    v.push_back(&create_node(dev, pattern));
  }

  return v;
}

ossia::net::address_scope get_address_scope(ossia::string_view addr)
{
  address_scope type = address_scope::relative;
  if (boost::starts_with(addr, "//"))
  {
    type = address_scope::relative;
  }
  else if (boost::starts_with(addr, "/"))
  {
    type = address_scope::absolute;
  }
  else
  {
    auto first_slash = addr.find("/");
    if (first_slash != std::string::npos && first_slash > 1
        && addr[first_slash - 1] == ':')
      type = address_scope::global;
  }
  return type;
}

std::vector<ossia::net::node_base*> list_all_children(ossia::net::node_base* node,
                                                      unsigned int depth)
{
  std::vector<ossia::net::node_base*> children = node->children_copy();
  std::vector<ossia::net::node_base*> list;

  static const std::regex pattern("\\.[0-9]+");

  // first sort by name taking care of instance number; i.e. foo.3 before foo.11
  ossia::sort(children, [&](auto n1, auto n2) {
    std::string s1 = n1->get_name();
    std::string s2 = n2->get_name();

    boost::algorithm::to_lower(s1);
    boost::algorithm::to_lower(s2);

    std::smatch asmatch, bsmatch;
    if(std::regex_search(s1, asmatch, pattern) && std::regex_search(s2, bsmatch, pattern))
    {
      if(asmatch.prefix() == bsmatch.prefix())
      {
        int aidx = std::stoi(asmatch.str(0).substr(1));
        int bidx = std::stoi(bsmatch.str(0).substr(1));
        return aidx < bidx;
      }
      else
      {
        return asmatch.prefix() < bsmatch.prefix();
      }
    }
    return s1 < s2;
  });

  // then sort by priority
  ossia::sort(children, [](auto n1, auto n2) {
    return ossia::net::get_priority(*n1) > ossia::net::get_priority(*n2);
  });

  int next_depth = -1;
  if(depth > 0)
    next_depth = depth - 1;

  for (auto it = children.begin(); it != children.end(); it++)
  {
    list.push_back(*it);
    if(next_depth != 0)
    {
      auto nested_list = list_all_children(*it, next_depth);
      list.insert(list.end(), nested_list.begin(), nested_list.end());
    }
  }

  return list;
}

/**
 * @brief fuzzysearch: search for nodes that match the pattern string
 * @param nodes: vector of nodes from where to start
 * @param pattern: strings to search
 * @return a vector of fuzzysearch_result sorted in descending score order
 */
void fuzzysearch(const std::vector<ossia::net::node_base*>& nodes,
                 const std::vector<std::string>& patterns,
                 std::vector<fuzzysearch_result>& results,
                 fuzzysearch_options opt)
{
  results.clear();

  for(const auto& node : nodes)
  {
    auto children = list_all_children(node);

    results.reserve(results.size() + children.size());

    for(const auto& n : children)
    {
      std::string oscaddress = ossia::net::osc_parameter_string_with_device(*n);
      if(!opt.case_sensitive)
      {
        // Make everything lowercase, address and patterns
        for(char& c : oscaddress)
          c = std::tolower(c);
        double percent = 1.0;

        auto res = patterns;
        for(std::string& s: res)
            for(char& c: s)
                c = std::tolower(c);

        for(const auto& pattern : res)
        {
          percent *= rapidfuzz::fuzz::partial_ratio(oscaddress, pattern) / 100.;
        }
        results.push_back({percent * 100., oscaddress, n});
      }
      else
      {
        double percent = 1.0;
        for(const auto& pattern : patterns)
        {
          percent *= rapidfuzz::fuzz::partial_ratio(oscaddress, pattern) / 100.;
        }
        results.push_back({percent * 100., oscaddress, n});
      }
    }

    // TODO in the future, when we'll use C++20
    // ranges::sort(results, std::greater{}, &fuzzysearch_result::score);
    ossia::sort(results, [](const fuzzysearch_result& left, const fuzzysearch_result& right){
      return left.score > right.score;
    });
  }
}

std::pair<std::vector<std::string>, bool> expand_address(const std::string& address)
{
  std::vector<std::string> names;
  bool pattern_matching = is_brace_expansion(address);

  if (pattern_matching)
  {
    // 1. Replace all [ ] with { } form
    auto str = canonicalize_str(address);

    // 2. Expand
    names = expand(str);
  }
  else
  {
    names.push_back(address);
  }

  return {names, pattern_matching};
}

std::vector<parameter_base*> find_or_create_parameter(
    node_base& node, const std::string& address, const std::string& type)
{
  // search for child that matches name but without parameter
  // and create parameter on that node if it exists
  // or create a new node with that name and a parameter

  auto [names, pattern_matching] = expand_address(address);

  std::vector<node_base*> nodes{};
  nodes.reserve(names.size());

  auto found_nodes = ossia::net::find_nodes(node, address);
  for (auto n : found_nodes)
  {
    if (n->get_parameter())
      // this will create a new node with the name incremented by one (e.g.
      // foo.1)
      nodes.push_back(&ossia::net::create_node(node, address));
    else
      nodes.push_back(n);

    if(pattern_matching)
      ossia::remove_one_if(names, [&](auto& s) { return s == n->get_name(); });
    else
      ossia::remove_one_if(names, [&](auto& s) { return s == address; });
  }

  for (auto s : names)
    nodes.push_back(&ossia::net::create_node(node, s));

  std::vector<parameter_base*> parameters{};
  parameters.reserve(nodes.size());

  for (auto n : nodes)
  {
    if (auto param = ossia::try_setup_parameter(type, *n))
      parameters.push_back(param);
  }

  return parameters;
}
}
}
