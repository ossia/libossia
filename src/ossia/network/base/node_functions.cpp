#include "node_functions.hpp"

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/path.hpp>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>

#if defined(OSSIA_HAS_RAPIDFUZZ)
#include <rapidfuzz/fuzz.hpp>
#endif

#if defined(OSSIA_HAS_CTRE)
#include <ctre.hpp>
#endif
#include <bitset>
#include <charconv>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
/*
  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Alteration:
  - simplify for the ascii usecase as we don't need locales, and only space is ' '.
  - put the case folding option at compile-time
*/
using nat_char = char;
static constexpr inline int nat_isdigit(nat_char a)
{
  return a >= '0' && a <= '9';
}

static constexpr inline int nat_isspace(nat_char a)
{
  return a == ' ';
}

static constexpr inline nat_char nat_toupper(nat_char a)
{
  return a >= 'a' && a <= 'z' ? a - ('a' - 'A') : a;
}

static constexpr int nat_compare_right(nat_char const* a, nat_char const* b)
{
  int bias = 0;

  /* The longest run of digits wins.  That aside, the greatest
     value wins, but we can't know that it will until we've scanned
     both numbers to know that they have the same magnitude, so we
     remember it in BIAS. */
  for(;; a++, b++)
  {
    if(!nat_isdigit(*a) && !nat_isdigit(*b))
      return bias;
    if(!nat_isdigit(*a))
      return -1;
    if(!nat_isdigit(*b))
      return +1;
    if(*a < *b)
    {
      if(!bias)
        bias = -1;
    }
    else if(*a > *b)
    {
      if(!bias)
        bias = +1;
    }
    else if(!*a && !*b)
      return bias;
  }

  return 0;
}

static constexpr int nat_compare_left(nat_char const* a, nat_char const* b)
{
  /* Compare two left-aligned numbers: the first to have a
     different value wins. */
  for(;; a++, b++)
  {
    if(!nat_isdigit(*a) && !nat_isdigit(*b))
      return 0;
    if(!nat_isdigit(*a))
      return -1;
    if(!nat_isdigit(*b))
      return +1;
    if(*a < *b)
      return -1;
    if(*a > *b)
      return +1;
  }

  return 0;
}

template <bool fold_case>
static constexpr int strnatcmp(nat_char const* a, nat_char const* b)
{
  int ai, bi;
  nat_char ca, cb;
  int fractional, result;

  ai = bi = 0;
  while(true)
  {
    ca = a[ai];
    cb = b[bi];

    /* skip over leading spaces or zeros */
    while(nat_isspace(ca))
      ca = a[++ai];

    while(nat_isspace(cb))
      cb = b[++bi];

    /* process run of digits */
    if(nat_isdigit(ca) && nat_isdigit(cb))
    {
      fractional = (ca == '0' || cb == '0');

      if(fractional)
      {
        if((result = nat_compare_left(a + ai, b + bi)) != 0)
          return result;
      }
      else
      {
        if((result = nat_compare_right(a + ai, b + bi)) != 0)
          return result;
      }
    }

    if(!ca && !cb)
    {
      /* The strings compare the same.  Perhaps the caller
         will want to call strcmp to break the tie. */
      return 0;
    }

    if constexpr(fold_case)
    {
      ca = nat_toupper(ca);
      cb = nat_toupper(cb);
    }

    if(ca < cb)
      return -1;

    if(ca > cb)
      return +1;

    ++ai;
    ++bi;
  }
}
}

namespace ossia::net
{
namespace
{
static node_base* find_node_rec(
    node_base& node,
    std::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if(first_slash_index != std::string::npos)
  {
    auto child = node.find_child(address.substr(0, first_slash_index));
    if(child)
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
    std::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if(first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if(cld)
    {
      // There are still nodes since we found a slash
      return find_or_create_node_rec(*cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = *node.create_child(std::string(cur));

      // Recurse on it
      return find_or_create_node_rec(child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // One of the child may be the researched node.
    auto n = node.find_child(address);
    if(n)
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
    std::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if(first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if(cld)
    {
      // There are still nodes since we found a slash
      return create_node_rec(*cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto child = node.create_child(std::string(cur));
      if(!child)
      {
#if defined(__cpp_exceptions)
        throw std::runtime_error{"create_node_rec: cannot create the node"};
#else
        std::terminate();
#endif
      }

      // Recurse on it
      return create_node_rec(*child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // Create and return the node
    auto child = node.create_child(std::string(address));
    if(!child)
    {
#if defined(__cpp_exceptions)
      throw std::runtime_error{"create_node_rec: cannot create the node"};
#else
      std::terminate();
#endif
    }
    return *child;
  }
}

//! Note : here we modify the string_view only.
//! The original address remains unchanged.
static std::string_view sanitize_address(std::string_view address)
{
  while(boost::algorithm::starts_with(address, "/"))
    address.remove_prefix(1);
  while(boost::algorithm::ends_with(address, "/"))
    address.remove_suffix(1);
  return address;
}
}

node_base* find_node(node_base& dev, std::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if(address.empty())
    return &dev;

  // address now looks like a/b/c
  return find_node_rec(dev, address);
}

node_base& find_or_create_node(node_base& node, std::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if(address.empty())
    return node;

  // address now looks like a/b/c
  return find_or_create_node_rec(node, address);
}

node_base& create_node(node_base& node, std::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if(address.empty())
    address = "node";

  // address now looks like a/b/c
  return create_node_rec(node, address);
}

node_base* find_or_create_node(node_base& dev, string_view parameter_base, bool reading)
{
  if(reading)
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
  if(!ossia::traversal::is_pattern(pattern))
  {
    auto node = ossia::net::find_node(root, pattern);
    if(node)
      return {node};
    else
      return {};
  }
  else if(auto path = traversal::make_path(pattern))
  {
    std::vector<node_base*> nodes{&root};
    traversal::apply(*path, nodes);
    return nodes;
  }
  else
  {
    auto node = ossia::net::find_node(root, pattern);
    if(node)
      return {node};
    else
      return {};
  }
}
}

#if defined(OSSIA_HAS_CTRE)
namespace ossia::net
{
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
      : _tbegin(begin)
      , _tend(begin)
      , _end(end)
  {
  }

  template <typename Lambda>
  bool next(Lambda istoken)
  {
    if(_tbegin == _end)
    {
      return false;
    }
    _tbegin = _tend;
    for(; _tend != _end && !istoken(*_tend); ++_tend)
    {
      if(*_tend == '\\' && std::next(_tend) != _end)
      {
        ++_tend;
      }
    }
    if(_tend == _tbegin && _tend != _end)
    {
      _tend++;
    }
    return _tbegin != _end;
  }

  ForwardIterator begin() const { return _tbegin; }
  ForwardIterator end() const { return _tend; }
  bool operator==(char c) { return *_tbegin == c; }
};

template <typename List>
void append_all(List& lista, const List& listb)
{
  if(listb.size() == 1)
  {
    for(auto& a : lista)
    {
      a += listb.back();
    }
  }
  else
  {
    List tmp;
    for(auto& a : lista)
    {
      for(auto& b : listb)
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

  while(token.next([](char c) { return c == '{' || c == ',' || c == '}'; }))
  {

    if(token == '{')
    {
      append_all(alts.back(), expand<String, List>(token));
    }
    else if(token == ',')
    {
      alts.push_back({String()});
    }
    else if(token == '}')
    {
      if(alts.size() == 1)
      {
        for(auto& a : alts.back())
        {
          a = '{' + a + '}';
        }
        return alts.back();
      }
      else
      {
        for(std::size_t i = 1; i < alts.size(); i++)
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
      for(auto& a : alts.back())
      {
        a.append(token.begin(), token.end());
      }
    }
  }

  List result{String{'{'}};
  append_all(result, alts.front());
  for(std::size_t i = 1; i < alts.size(); i++)
  {
    for(auto& a : result)
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
    typename String
    = std::basic_string<typename std::iterator_traits<ForwardIterator>::value_type>,
    typename List = std::vector<String>>
List expand(ForwardIterator begin, ForwardIterator end)
{
  detail::tokenizer<ForwardIterator> token(begin, end);
  List list{String()};
  while(token.next([](char c) { return c == '{'; }))
  {
    if(token == '{')
    {
      detail::append_all(list, detail::expand<String, List>(token));
    }
    else
    {
      for(auto& a : list)
      {
        a.append(token.begin(), token.end());
      }
    }
  }
  return list;
}

template <
    typename Range, typename String = std::basic_string<typename Range::value_type>,
    typename List = std::vector<String>>
List expand(const Range& range)
{
  using Iterator = typename Range::const_iterator;
  return expand<Iterator, String, List>(std::begin(range), std::end(range));
}

static constexpr auto expand_ranges_3_regex
    = ctll::fixed_string{R"_(\{(-?[0-9]+)\.\.(-?[0-9]+)\.\.(-?[0-9]+)\})_"};
static constexpr auto expand_ranges_2_regex
    = ctll::fixed_string{R"_(\{(-?[0-9]+)\.\.(-?[0-9]+)\})_"};

static constexpr auto reg_alpha_low_rex
    = ctll::fixed_string{R"_(\{(-?[a-z])\.\.(-?[a-z])\})_"};
static constexpr auto reg_alpha_up_rex
    = ctll::fixed_string{R"_(\{(-?[A-Z])\.\.(-?[A-Z])\})_"};
void expand_ranges(std::string& str)
{
  if(str.find('{') != std::string::npos)
  {
    {
      struct rx_triple
      {
        std::size_t start{}, length{};
        int64_t first{}, last{}, increment{};
      };
      ossia::small_vector<rx_triple, 4> positions;

      {
        constexpr auto rex = ctre::range<expand_ranges_3_regex>;
        for(auto& it : rex(str))
        {
          int fst = it.get<1>().to_number();
          int lst = it.get<2>().to_number();
          int inc = it.get<3>().to_number();
          if(inc != 0)
          {
            positions.push_back(rx_triple{
                (std::size_t)(it.get<0>().data() - str.data()), (std::size_t)it.size(),
                std::min(fst, lst), std::max(fst, lst), inc});
          }
        }
      }

      for(auto it = positions.rbegin(); it != positions.rend(); ++it)
      {
        std::string rep{"{"};
        rep.reserve(3 * std::abs((it->last - it->first) / it->increment));
        if(it->increment > 0)
        {
          for(int64_t v = it->first; v <= it->last; v += it->increment)
          {
            rep += std::to_string(v);
            rep += ',';
          }
        }
        else
        {
          for(int64_t v = it->last; v <= it->first; v += it->increment)
          {
            rep += std::to_string(v);
            rep += ',';
          }
        }

        if(rep.back() == ',')
        {
          rep.back() = '}';
          str.replace(it->start, it->length, rep);
        }
      }
    }

    {
      struct rx_double
      {
        std::size_t start{}, length{};
        int64_t first{}, last{};
      };
      ossia::small_vector<rx_double, 4> positions;

      {
        constexpr auto rex = ctre::range<expand_ranges_2_regex>;
        for(auto& it : rex.range(str))
        {
          int fst = it.get<1>().to_number();
          int lst = it.get<2>().to_number();
          positions.push_back(rx_double{
              (std::size_t)(it.get<0>().data() - str.data()), (std::size_t)it.size(),
              std::min(fst, lst), std::max(fst, lst)});
        }
      }

      for(auto it = positions.rbegin(); it != positions.rend(); ++it)
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
          for(int64_t v = it->first; v <= it->last; v++)
          {
            rep += std::to_string(v);
            rep += ',';
          }

          if(rep.back() == ',')
          {
            rep.back() = '}';
            str.replace(it->start, it->length, rep);
          }
        }
      }
    }

    {
      constexpr auto reg_alpha_low = ctre::range<reg_alpha_low_rex>;
      constexpr auto reg_alpha_up = ctre::range<reg_alpha_up_rex>;

      struct rx_char
      {
        std::size_t start{}, length{};
        char first{}, last{};
      };
      auto apply = [&](auto& reg) {
        ossia::small_vector<rx_char, 4> positions;

        for(auto& it : reg(str))
        {
          char fst = it.template get<1>().data()[0];
          char lst = it.template get<2>().data()[0];
          positions.push_back(rx_char{
              (std::size_t)(it.template get<0>().data() - str.data()),
              (std::size_t)it.size(), std::min(fst, lst), std::max(fst, lst)});
        }

        for(auto it = positions.rbegin(); it != positions.rend(); ++it)
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
            for(char v = it->first; v <= it->last; v++)
            {
              rep += v;
              rep += ',';
            }

            if(rep.back() == ',')
            {
              rep.back() = '}';
              str.replace(it->start, it->length, rep);
            }
          }
        }
      };
      apply(reg_alpha_low);
      apply(reg_alpha_up);
    }
  }
}

static constexpr auto canonicalize_regex = ctll::fixed_string{R"(\[[a-zA-Z0-9\-]+\])"};
std::string canonicalize_str(std::string str)
{
  {
    // 1. find all [a-z0-9XYZ]
    std::replace(str.begin(), str.end(), '|', ',');

    struct rx_pos
    {
      std::size_t start{}, length{};
      std::bitset<128> chars;
    };
    ossia::small_vector<rx_pos, 4> positions;

    constexpr auto rx_class = ctre::range<canonicalize_regex>;
    for(auto& it : rx_class(str))
    {
      const auto theStr = it.view().substr(1, it.size() - 2);
      std::bitset<128> bits;

      for(int i = 0, N = theStr.size(); i < N;)
      {
        if((N - i) > 2)
        {
          if(theStr[i + 1] == '-' && (int)theStr[i + 2] > (int)theStr[i])
          {
            for(int ch = (int)theStr[i]; ch <= (int)theStr[i + 2]; ++ch)
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
      positions.push_back(
          rx_pos{(std::size_t)(it.data() - str.data()), (std::size_t)it.size(), bits});
    }

    for(auto it = positions.rbegin(); it != positions.rend(); ++it)
    {
      std::string rep{"{"};
      for(std::size_t i = 0; i < it->chars.size(); ++i)
      {
        if(it->chars[i])
        {
          rep += (char)i;
          rep += ',';
        }
      }

      if(rep.back() == ',')
      {
        rep.back() = '}';
        str.replace(it->start, it->length, rep);
      }
    }
  }

  expand_ranges(str);
  return str;
}
}
#else
namespace ossia::net
{
// FIXME
std::vector<std::string> expand(const std::string& range)
{
  return {range};
}

// FIXME
std::string canonicalize_str(std::string str)
{
  return str;
}
void expand_ranges(std::string& str) { }
}
#endif

namespace ossia::net
{
std::vector<node_base*> create_nodes(node_base& dev, string_view pattern)
{
  std::vector<node_base*> v;

  if(is_brace_expansion(pattern))
  {
    // 1. Replace all [ ] with { } form
    auto str = canonicalize_str(std::string(pattern));

    // 2. Expand
    auto expanded = expand(str);

    // 3. Create nodes
    v.reserve(expanded.size());
    for(const auto& addr : expanded)
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

ossia::net::address_scope get_address_scope(std::string_view addr)
{
  address_scope type = address_scope::relative;
  if(boost::starts_with(addr, "//"))
  {
    type = address_scope::relative;
  }
  else if(boost::starts_with(addr, "/"))
  {
    type = address_scope::absolute;
  }
  else
  {
    auto first_slash = addr.find("/");
    if(first_slash != std::string::npos && first_slash > 1
       && addr[first_slash - 1] == ':')
      type = address_scope::global;
  }
  return type;
}

static bool node_sort(ossia::net::node_base* n1, ossia::net::node_base* n2)
{
  const auto& s1 = n1->get_name();
  const auto& s2 = n2->get_name();
  int order = strnatcmp<true>(s1.data(), s2.data());
  if(order < 0)
    return true;
  else if(order == 0)
    return ossia::net::get_priority(*n1) > ossia::net::get_priority(*n2);
  else
    return false;
}

template<bool sort>
static void list_all_children_rec(
    ossia::net::node_base* node, unsigned int depth,
    std::vector<ossia::net::node_base*>& list)
{
  std::vector<ossia::net::node_base*> children = node->children_copy();

  if constexpr(sort)
    ossia::sort(children, node_sort);

  int next_depth = -1;
  if(depth > 0)
  {
    next_depth = depth;
    next_depth--;
  }

  for(auto* child : children)
  {
    list.push_back(child);
    if(next_depth != 0)
    {
      list_all_children_rec<sort>(child, next_depth, list);
    }
  }
}

template<bool sort>
static void list_all_children_rec(
    ossia::net::node_base* node, std::vector<ossia::net::node_base*>& list)
{
  std::vector<ossia::net::node_base*> children = node->children_copy();

  if constexpr(sort)
    ossia::sort(children, node_sort);

  for(auto* child : children)
  {
    list.push_back(child);
    list_all_children_rec<sort>(child, list);
  }
}


void list_all_children(
        ossia::net::node_base* node,
        std::vector<ossia::net::node_base*>& list,
        unsigned int depth, bool sort)
{
  if(depth == 0) // No limit
  {
    if(sort)
      list_all_children_rec<true>(node, list);
    else
      list_all_children_rec<false>(node, list);
  }
  else
  {
    if(sort)
      list_all_children_rec<true>(node, depth, list);
    else
      list_all_children_rec<false>(node, depth, list);
  }
}

std::vector<ossia::net::node_base*>
list_all_children(ossia::net::node_base* node, unsigned int depth, bool sort)
{
  std::vector<ossia::net::node_base*> list;
  list.reserve(250);
  list_all_children(node, list, depth, sort);
  return list;
}

void iterate_all_children(
    ossia::net::node_base* node,
    const std::function<void(ossia::net::parameter_base&)>& f)
{
  if(auto p = node->get_parameter())
    f(*p);
  for(auto& cld : node->children())
    iterate_all_children(cld.get(), f);
}

/**
 * @brief fuzzysearch: search for nodes that match the pattern string
 * @param nodes: vector of nodes from where to start
 * @param pattern: strings to search
 * @return a vector of fuzzysearch_result sorted in descending score order
 */
void fuzzysearch(
    const std::vector<ossia::net::node_base*>& nodes,
    const std::vector<std::string>& patterns, std::vector<fuzzysearch_result>& results,
    fuzzysearch_options opt)
{
#if defined(OSSIA_HAS_RAPIDFUZZ)
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
        for(std::string& s : res)
          for(char& c : s)
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
    ossia::sort(
        results, [](const fuzzysearch_result& left, const fuzzysearch_result& right) {
          return left.score > right.score;
        });
  }
#endif
}

std::pair<std::vector<std::string>, bool> expand_address(std::string address)
{
  std::vector<std::string> names;
  bool pattern_matching = is_brace_expansion(address);

  if(pattern_matching)
  {
    // 1. Replace all [ ] with { } form
    auto str = canonicalize_str(std::move(address));

    // 2. Expand
    names = expand(str);
  }
  else
  {
    names.push_back(std::move(address));
  }

  return {std::move(names), pattern_matching};
}

parameter_base* find_or_create_parameter(
    node_base& node, std::string_view address, std::string_view type)
{
  auto& n = ossia::net::find_or_create_node(node, address);
  {
    if(auto param = n.get_parameter())
      return param;
    else if(auto param = ossia::try_setup_parameter(type, n))
      return param;
    else
      return nullptr;
  }
}

std::vector<parameter_base*> find_parameter_or_create_node(
    node_base& node, std::string_view address, std::string_view type)
{
  // search for child that matches name but without parameter
  // and create parameter on that node if it exists
  // or create a new node with that name and a parameter

  auto [names, pattern_matching] = expand_address(std::string(address));

  std::vector<node_base*> nodes{};
  nodes.reserve(names.size());

  auto found_nodes = ossia::net::find_nodes(node, address);
  for(auto n : found_nodes)
  {
    if(n->get_parameter())
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

  for(const auto& s : names)
    nodes.push_back(&ossia::net::create_node(node, s));

  std::vector<parameter_base*> parameters{};
  parameters.reserve(nodes.size());

  for(auto n : nodes)
  {
    if(auto param = ossia::try_setup_parameter(type, *n))
      parameters.push_back(param);
  }

  return parameters;
}

value_map_type to_map(const node_base& n) noexcept
{
  ossia::value_map_type map;
  const auto& cld = n.children();
  for(auto& node : cld)
  {
    auto& e = map[node->get_name()];
    if(node->children().size() > 0)
    {
      e = to_map(*node);
    }
    else if(auto p = node->get_parameter())
    {
      e = p->value();
    }
    else
    {
      e = ossia::value{};
    }
  }
  return map;
}
}
