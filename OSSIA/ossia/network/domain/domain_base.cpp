#include "domain_base.hpp"

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

}
