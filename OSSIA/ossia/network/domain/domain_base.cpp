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

value domain_base<Tuple>::clamp(bounding_mode b, const Tuple& val) const
{
  // TODO Tuple domain is in fact some kind of generic_domain.
  // Instead of having the for-loops here, we should have them in domain_clamp_visitor.
  // Same for vec.
  if (b == bounding_mode::FREE)
    return val;

  if(values.empty())
  {
    const bool has_min = bool(min);
    const bool has_max = bool(max);
    if (has_min && has_max)
    {
      ossia::Tuple res;
      res.value.reserve(val.value.size());

      switch (b)
      {
        case bounding_mode::CLIP:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp(v, min.get(), max.get()));
          }
          break;
        case bounding_mode::WRAP:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::wrap(v, min.get(), max.get()));
          }
          break;
        case bounding_mode::FOLD:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::fold(v, min.get(), max.get()));
          }
          break;
        case bounding_mode::LOW:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_min(v, min.get()));
          }
          break;
        case bounding_mode::HIGH:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_max(v, max.get()));
          }
          break;
        default:
          return val;
      }

      return res;
    }
    else if (has_min)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          ossia::Tuple res;
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_min(v, min.get()));
          }
          return res;
        }
        default:
          return val;
      }
    }
    else if (has_max)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          ossia::Tuple res;
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_max(v, max.get()));
          }
          return res;
        }
        default:
          return val;
      }
    }
    else
    {
      return val;
    }
  }
  else
  {
    // Return a valid value only if it is in the given values
    auto it = values.find(val.value);
    if (it != values.end())
    {
      return val;
    }
    else
    {
      return ossia::value{};
    }
  }
}

value domain_base<Tuple>::clamp(bounding_mode b, Tuple&& val) const
{
  if (b == bounding_mode::FREE)
    return std::move(val);

  if(values.empty())
  {
    const bool has_min = bool(min);
    const bool has_max = bool(max);
    if (has_min && has_max)
    {
      switch (b)
      {
        case bounding_mode::CLIP:
          for(auto& v : val.value)
          {
            v = ossia::clamp(v, min.get(), max.get());
          }
          break;
        case bounding_mode::WRAP:
          for(auto& v : val.value)
          {
            v = ossia::wrap(v, min.get(), max.get());
          }
          break;
        case bounding_mode::FOLD:
          for(auto& v : val.value)
          {
            v = ossia::fold(v, min.get(), max.get());
          }
          break;
        case bounding_mode::LOW:
          for(auto& v : val.value)
          {
            v = ossia::clamp_min(v, min.get());
          }
          break;
        case bounding_mode::HIGH:
          for(auto& v : val.value)
          {
            v = ossia::clamp_max(v, max.get());
          }
          break;
        default:
          return std::move(val);
      }

      return std::move(val);
    }
    else if (has_min)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          for(auto& v : val.value)
          {
            v = ossia::clamp_min(v, min.get());
          }
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else if (has_max)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          for(auto& v : val.value)
          {
            v = ossia::clamp_max(v, max.get());
          }
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else
    {
      return std::move(val);
    }
  }
  else
  {
    // Return a valid value only if it is in the given values
    auto it = values.find(val.value);
    if (it != values.end())
    {
      return val;
    }
    else
    {
      return ossia::value{};
    }
  }
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


template<int N>
struct arraylike_domain_clamp_simple
{
  using vec = Vec<float, N>;
  const domain_base<vec>& domain;

  ossia::value operator()(bounding_mode b, vec val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const auto min = domain.min.get();
      const auto max = domain.max.get();
      switch (b)
      {
        case bounding_mode::CLIP:
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::clamp(val.value[i], min, max);
          }
          break;
        case bounding_mode::WRAP:
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::wrap(val.value[i], min, max);
          }
          break;
        case bounding_mode::FOLD:
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::fold(val.value[i], min, max);
          }
          break;
        case bounding_mode::LOW:
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::clamp_min(val.value[i], min);
          }
          break;
        case bounding_mode::HIGH:
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::clamp_max(val.value[i], max);
          }
          break;
        default:
          return val;
      }

      return val;
    }
    else if (has_min)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::clamp_min(val.value[i], domain.min.get());
          }
          return val;
        }
        default:
          return val;
      }
    }
    else if (has_max)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          for(int i = 0; i < N; i++)
          {
            val.value[i] = ossia::clamp_max(val.value[i], domain.max.get());
          }
          return val;
        }
        default:
          return val;
      }
    }
    else
    {
      return val;
    }
  }
};

template<>
OSSIA_EXPORT value domain_base<Vec2f>::clamp(bounding_mode b, const Vec2f& val) const
{
  return arraylike_domain_clamp_simple<val.size_value>{*this}(b, val);
}

template<>
OSSIA_EXPORT value domain_base<Vec3f>::clamp(bounding_mode b, const Vec3f& val) const
{
  return arraylike_domain_clamp_simple<val.size_value>{*this}(b, val);
}

template<>
OSSIA_EXPORT value domain_base<Vec4f>::clamp(bounding_mode b, const Vec4f& val) const
{
  return arraylike_domain_clamp_simple<val.size_value>{*this}(b, val);
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
