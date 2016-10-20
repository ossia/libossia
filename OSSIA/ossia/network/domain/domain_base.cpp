#include "domain_base.hpp"
#include <ossia/network/domain/detail/domain_visitors.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{

struct clamp_visitor
{
  template<typename T, typename U, typename V>
  ossia::value operator()(T&& val, const U& min, const V& max)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{ossia::clamp(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{ossia::clamp(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{ossia::clamp(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{ossia::clamp(val.value, min.value, max.value)}; }
};

struct clamp_min_visitor
{
  template<typename T, typename U>
  ossia::value operator()(T&& val, const U& min)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int min)
  { return Int{ossia::clamp_min(val.value, min.value)}; }
  ossia::value operator()(Float val, Float min)
  { return Float{ossia::clamp_min(val.value, min.value)}; }
  ossia::value operator()(Char val, Char min)
  { return Char{ossia::clamp_min(val.value, min.value)}; }
  ossia::value operator()(Bool val, Bool min)
  { return Bool{ossia::clamp_min(val.value, min.value)}; }
};

struct clamp_max_visitor
{
  template<typename T, typename U>
  ossia::value operator()(T&& val, const U& max)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int max)
  { return Int{ossia::clamp_max(val.value, max.value)}; }
  ossia::value operator()(Float val, Float max)
  { return Float{ossia::clamp_max(val.value, max.value)}; }
  ossia::value operator()(Char val, Char max)
  { return Char{ossia::clamp_max(val.value, max.value)}; }
  ossia::value operator()(Bool val, Bool max)
  { return Bool{ossia::clamp_max(val.value, max.value)}; }
};

struct wrap_visitor
{
  template<typename T, typename U, typename V>
  ossia::value operator()(T&& val, const U& min, const V& max)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{ossia::wrap(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{ossia::wrap(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{ossia::wrap(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{ossia::wrap(val.value, min.value, max.value)}; }
};

struct fold_visitor
{
  template<typename T, typename U, typename V>
  ossia::value operator()(T&& val, const U& min, const V& max)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{ossia::fold(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{ossia::fold(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{ossia::fold(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{ossia::fold(val.value, min.value, max.value)}; }
};


/// Const-reference overloads
ossia::value clamp(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(clamp_visitor{}, val.v, min.v, max.v);
}

ossia::value clamp_min(const ossia::value& val, const ossia::value& min)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid())
    return val;
  return eggs::variants::apply(clamp_min_visitor{}, val.v, min.v);
}

ossia::value clamp_max(const ossia::value& val, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe max ?
  if(!max.valid())
    return val;
  return eggs::variants::apply(clamp_max_visitor{}, val.v, max.v);
}

ossia::value wrap(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(wrap_visitor{}, val.v, min.v, max.v);
}

ossia::value fold(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(fold_visitor{}, val.v, min.v, max.v);
}

/// Move-overloads
ossia::value clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(clamp_visitor{}, std::move(val).v, min.v, max.v);
}

ossia::value clamp_min(ossia::value&& val, const ossia::value& min)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid())
    return val;
  return eggs::variants::apply(clamp_min_visitor{}, std::move(val).v, min.v);
}

ossia::value clamp_max(ossia::value&& val, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe max ?
  if(!max.valid())
    return val;
  return eggs::variants::apply(clamp_max_visitor{}, std::move(val).v, max.v);
}

ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(wrap_visitor{}, std::move(val).v, min.v, max.v);
}

ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(fold_visitor{}, std::move(val).v, min.v, max.v);
}


namespace net
{
value get_min(const domain& dom)
{
  return ossia::apply(domain_min_visitor{}, dom);
}

value get_max(const domain& dom)
{
  return ossia::apply(domain_max_visitor{}, dom);
}

void set_min(domain& dom, const ossia::value& val)
{
  return ossia::apply(domain_set_min_visitor{val}, dom);
}

void set_max(domain& dom, const ossia::value& val)
{
  return ossia::apply(domain_set_max_visitor{val}, dom);
}

domain make_domain(const ossia::value& min, const ossia::value& max)
{
  if (min.valid() && max.valid())
  {
    return eggs::variants::apply(domain_minmax_creation_visitor{}, min.v, max.v);
  }
  return {};
}

domain make_domain(
    const boost::container::small_vector<ossia::value, 2>& val,
    const ossia::value& cur)
{
  if (val.size() == 2 && val[0].valid() && val[1].valid())
  {
    if(cur.getType() != val_type::TUPLE)
    {
      return eggs::variants::apply(domain_minmax_creation_visitor{}, val[0].v, val[1].v);
    }
    else
    {
      return ossia::net::domain_base<Tuple>{val[0], val[1], boost::container::flat_set<Tuple>{}};
    }
  }
  else
  {
    for(auto& val : v)
    {

    }
  }

  if (min.valid() && max.valid())
  {
    return eggs::variants::apply(domain_minmax_creation_visitor{}, min.v, max.v);
  }
  return {};
}

bool
operator==(const domain_base<Int>& lhs, const domain_base<Int>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Float>& lhs, const domain_base<Float>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Bool>& lhs, const domain_base<Bool>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Char>& lhs, const domain_base<Char>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<String>& lhs, const domain_base<String>& rhs)
{
  return lhs.values == rhs.values;
}

value clamp(const domain& dom, bounding_mode b, const ossia::value& val)
{
  if (bool(dom) && bool(val.v))
  {
    return eggs::variants::apply(domain_clamp_visitor{b}, val.v, dom);
  }
  return val;
}

value clamp(const domain& dom, bounding_mode b, ossia::value&& val)
{
  if (bool(dom) && bool(val.v))
  {
    return eggs::variants::apply(domain_clamp_visitor{b}, std::move(val.v), dom);
  }
  return val;
}

value domain_base<String>::clamp(bounding_mode b, const String& val) const
{
  if (values.empty())
  {
    return val;
  }
  else
  {
    auto it = values.find(val.value);
    if (it != values.end())
    {
      return String(*it);
    }
    else
    {
      return ossia::value{};
    }
  }
}

template<>
OSSIA_EXPORT value domain_base<Vec2f>::clamp(bounding_mode b, const Vec2f& val) const
{
}

template<>
OSSIA_EXPORT value domain_base<Vec3f>::clamp(bounding_mode b, const Vec3f& val) const
{
}

template<>
OSSIA_EXPORT value domain_base<Vec4f>::clamp(bounding_mode b, const Vec4f& val) const
{
}

domain init_domain(ossia::val_type type)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return domain_base<Impulse>{};
    case val_type::INT:
      return domain_base<Int>{};
    case val_type::FLOAT:
      return domain_base<Float>{};
    case val_type::BOOL:
      return domain_base<Bool>{};
    case val_type::CHAR:
      return domain_base<Char>{};
    case val_type::STRING:
      return domain_base<String>();
    case val_type::TUPLE:
      return domain_base<Tuple>();
    case val_type::VEC2F:
      return domain_base<Vec2f>();
    case val_type::VEC3F:
      return domain_base<Vec3f>();
    case val_type::VEC4F:
      return domain_base<Vec4f>();
    case val_type::DESTINATION:
      return domain_base<Destination>{};
    case val_type::BEHAVIOR:
      return domain_base<Behavior>{};
    default:
      return domain{};
  }
}





/*
template<>
domain_min_max<Int> min_and_max(const domain& dom)
{ return ossia::apply(domain_min_max_visitor<Int>{}, dom); }
template<>
domain_min_max<Float> min_and_max(const domain& dom)
{ return ossia::apply(domain_min_max_visitor<Float>{}, dom); }
template<>
domain_min_max<Bool> min_and_max(const domain& dom)
{ return ossia::apply(domain_min_max_visitor<Bool>{}, dom); }
template<>
domain_min_max<Char> min_and_max(const domain& dom)
{ return ossia::apply(domain_min_max_visitor<Char>{}, dom); }
template<>
domain_min_max<Tuple> min_and_max(const domain& dom)
{ return ossia::apply(domain_min_max_visitor<Tuple>{}, dom); }
*/
}
}
