#include "node_functions.hpp"
#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <chobo/small_vector.hpp>


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
      if(!child)
        throw std::runtime_error{"create_node_rec: cannot create the node"};

      // Recurse on it
      return create_node_rec(*child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // Create and return the node
    auto child = node.create_child(std::string(address));
    if(!child)
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
find_or_create_node(node_base& dev, string_view address_base, bool create)
{
  if (create)
  {
    return ossia::net::find_node(dev, address_base);
  }
  else
  {
    return &ossia::net::create_node(dev, address_base);
  }
}

std::vector<node_base*> find_nodes(node_base& dev, string_view pattern)
{
  if(auto path = traversal::make_path(std::string(pattern)))
  {
    std::vector<node_base*> nodes{&dev};
    traversal::apply(*path, nodes);
    return nodes;
  }
  else
  {
    auto node = ossia::net::find_node(dev, pattern);
    if(node)
      return {node};
    else
      return {};
  }
}

// The following code, up to and including `List expand(const Range & range)`
// originates from Rosetta Code, under GNU FDL:
// https://rosettacode.org/wiki/Brace_expansion#C.2B.2B
namespace detail {

template <typename ForwardIterator>
class tokenizer
{

    ForwardIterator _tbegin, _tend, _end;

  public:

    tokenizer(ForwardIterator begin, ForwardIterator end)
      : _tbegin(begin), _tend(begin), _end(end)
    { }

    template <typename Lambda>
    bool next(Lambda istoken)
    {
      if (_tbegin == _end) {
        return false;
      }
      _tbegin = _tend;
      for (; _tend != _end && !istoken(*_tend); ++_tend) {
        if (*_tend == '\\' && std::next(_tend) != _end) {
          ++_tend;
        }
      }
      if (_tend == _tbegin) {
        _tend++;
      }
      return _tbegin != _end;
    }

    ForwardIterator begin() const { return _tbegin; }
    ForwardIterator end()   const { return _tend; }
    bool operator==(char c) { return *_tbegin == c; }

};

template <typename List>
void append_all(List & lista, const List & listb)
{
  if (listb.size() == 1) {
    for (auto & a : lista) {
      a += listb.back();
    }
  } else {
    List tmp;
    for (auto & a : lista) {
      for (auto & b : listb) {
        tmp.push_back(a + b);
      }
    }
    lista = std::move(tmp);
  }
}

template <typename String, typename List, typename Tokenizer>
List expand(Tokenizer & token)
{

  std::vector<List> alts{ { String() } };

  while (token.next([](char c) { return c == '{' || c == ',' || c == '}'; })) {

    if (token == '{') {
      append_all(alts.back(), expand<String, List>(token));
    } else if (token == ',') {
      alts.push_back({ String() });
    } else if (token == '}') {
      if (alts.size() == 1) {
        for (auto & a : alts.back()) {
          a = '{' + a + '}';
        }
        return alts.back();
      } else {
        for (std::size_t i = 1; i < alts.size(); i++) {
          alts.front().insert(alts.front().end(),
                              std::make_move_iterator(std::begin(alts[i])),
                              std::make_move_iterator(std::end(alts[i])));
        }
        return std::move(alts.front());
      }
    } else {
      for (auto & a : alts.back()) {
        a.append(token.begin(), token.end());
      }
    }

  }

  List result{ String{ '{' } };
  append_all(result, alts.front());
  for (std::size_t i = 1; i < alts.size(); i++) {
    for (auto & a : result) {
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
      typename std::iterator_traits<ForwardIterator>::value_type
      >,
    typename List = std::vector<String>
    >
List expand(ForwardIterator begin, ForwardIterator end)
{
  detail::tokenizer<ForwardIterator> token(begin, end);
  List list{ String() };
  while (token.next([](char c) { return c == '{'; })) {
    if (token == '{') {
      detail::append_all(list, detail::expand<String, List>(token));
    } else {
      for (auto & a : list) {
        a.append(token.begin(), token.end());
      }
    }
  }
  return list;
}

template <
    typename Range,
    typename String = std::basic_string<typename Range::value_type>,
    typename List = std::vector<String>
    >
List expand(const Range & range)
{
  using Iterator = typename Range::const_iterator;
  return expand<Iterator, String, List>(std::begin(range), std::end(range));
}

std::string canonicalize_str(std::string str)
{
  // 1. find all [a-z0-9XYZ]
  static std::regex rx_class{"\\[[a-zA-Z0-9-]+\\]"};

  struct rx_pos
  {
      std::size_t start{}, length{};
      std::bitset<128> chars;
  };
  chobo::small_vector<rx_pos, 4> positions;

  std::regex_iterator<std::string::iterator> rit ( str.begin(), str.end(), rx_class);
  std::regex_iterator<std::string::iterator> rend;

  for(auto it = rit; it != rend; ++it)
  {
    auto str = it->str();
    str = str.substr(1, str.size() - 2);
    std::bitset<128> bits;

    for(int i = 0, N = str.size(); i < N; )
    {
      if((N - i) > 2)
      {
        if(str[i+1] == '-' && (int) str[i+2] > (int) str[i])
        {
          for(int ch = (int) str[i]; ch <= (int) str[i+2]; ++ch)
            bits[ch] = true;
          i += 2;
        }
        else
        {
          bits[(int) str[i]] = true;
          i++;
        }
      }
      else
      {
        bits[(int) str[i]] = true;
        i++;
      }
    }
    positions.push_back(rx_pos{(std::size_t)it->position(), (std::size_t)it->length(), bits});
  }

  for(auto it = positions.rbegin(); it != positions.rend(); ++it)
  {
    std::string rep{"{"};
    for(std::size_t i = 0; i < it->chars.size(); ++i)
    {
      if(it->chars[i])
      {
        rep += (char) i;
        rep += ',';
      }
    }

    if(rep.back() == ',')
    {
      rep.back() = '}';
      str.replace(it->start, it->length, rep);
    }
  }

  return str;
}

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

}
}
