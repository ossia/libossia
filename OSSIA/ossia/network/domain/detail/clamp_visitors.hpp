#pragma once
#include <ossia/editor/value/value.hpp>

namespace ossia
{
// For clamp, wrap, fold...
template<typename TernaryFun>
struct apply_ternary_fun_visitor
{
  TernaryFun f;
  template<typename T, typename U, typename V>
  OSSIA_INLINE ossia::value operator()(const T& val, const U& min, const V& max)
  { return val; }
  template<typename U, typename V>
  OSSIA_INLINE ossia::value operator()(Tuple&& val, const U& min, const V& max)
  { return std::move(val); }
  template<typename U, typename V>
  OSSIA_INLINE ossia::value operator()(const Tuple& val, const U& min, const V& max)
  { return val; }
  template<typename U, typename V>
  OSSIA_INLINE ossia::value operator()(String&& val, const U& min, const V& max)
  { return std::move(val); }
  template<typename U, typename V>
  OSSIA_INLINE ossia::value operator()(const String& val, const U& min, const V& max)
  { return val; }

  OSSIA_INLINE ossia::value operator()(Int val, Int min, Int max)
  { return Int{f(val, min, max)}; }
  OSSIA_INLINE ossia::value operator()(Float val, Float min, Float max)
  { return Float{f(val, min, max)}; }
  OSSIA_INLINE ossia::value operator()(Char val, Char min, Char max)
  { return Char{f(val, min, max)}; }
  OSSIA_INLINE ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{f(val, min, max)}; }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, const Vec<float, N>& min, const Vec<float, N>& max)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val[i] = f(val[i], min[i], max[i]);
    }
    return val;
  }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, Float min, Float max)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val[i] = f(val[i], min, max);
    }
    return val;
  }

  // TODO handle clamping of Tuple between two values of other types
  // (see apply_domain.hpp)
  ossia::value operator()(const Tuple& incoming, const Tuple& min, const Tuple& max)
  {
    Tuple val;
    const auto N = incoming.size();
    const auto nmin = min.size();
    const auto nmax = max.size();
    if(N == nmin && N == nmax)
    {
      val.reserve(N);
      for(std::size_t i = 0; i < N; i++)
      {
        val.push_back(eggs::variants::apply(*this, incoming[i].v, min[i].v, max[i].v));
      }
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const Tuple& min, const Tuple& max)
  {
    const auto N = val.size();
    const auto nmin = min.size();
    const auto nmax = max.size();
    if(N == nmin && N == nmax)
    {
      for(std::size_t i = 0; i < N; i++)
      {
        val[i] = eggs::variants::apply(*this, std::move(val)[i].v, min[i].v, max[i].v);
      }
    }
    return std::move(val);
  }

  ossia::value operator()(const Tuple& incoming, const ossia::value& min, const ossia::value& max)
  {
    Tuple val;
    const auto N = incoming.size();
    val.reserve(N);
    for(std::size_t i = 0; i < N; i++)
    {
      val.push_back(eggs::variants::apply(*this, incoming[i].v, min.v, max.v));
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const ossia::value& min, const ossia::value& max)
  {
    const auto N = val.size();
    for(std::size_t i = 0; i < N; i++)
    {
      val[i] = eggs::variants::apply(*this, std::move(val)[i].v, min.v, max.v);
    }
    return std::move(val);
  }

  ossia::value operator()(const ossia::value& val, const ossia::value& min, const ossia::value& max)
  {
    return eggs::variants::apply(*this, val.v, min.v, max.v);
  }

  ossia::value operator()(ossia::value&& val, const ossia::value& min, const ossia::value& max)
  {
    return eggs::variants::apply(*this, std::move(val.v), min.v, max.v);
  }
};

// For clamp_min, clamp_max...
template<typename BinaryFun>
struct apply_binary_fun_visitor
{
  BinaryFun f;

  template<typename T, typename U>
  OSSIA_INLINE ossia::value operator()(const T& val, const U& min)
  { return val; }
  template<typename U>
  OSSIA_INLINE ossia::value operator()(Tuple&& val, const U& min)
  { return std::move(val); }
  template<typename U>
  OSSIA_INLINE ossia::value operator()(const Tuple& val, const U& min)
  { return val; }
  template<typename U>
  OSSIA_INLINE ossia::value operator()(String&& val, const U& min)
  { return std::move(val); }
  template<typename U>
  OSSIA_INLINE ossia::value operator()(const String& val, const U& min)
  { return val; }

  OSSIA_INLINE ossia::value operator()(Int val, Int min)
  { return Int{f(val, min)}; }
  OSSIA_INLINE ossia::value operator()(Float val, Float min)
  { return Float{f(val, min)}; }
  OSSIA_INLINE ossia::value operator()(Char val, Char min)
  { return Char{f(val, min)}; }
  OSSIA_INLINE ossia::value operator()(Bool val, Bool min)
  { return Bool{f(val, min)}; }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, const Vec<float, N>& min)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val[i] = f(val[i], min[i]);
    }
    return val;
  }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, Float min)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val[i] = f(val[i], min);
    }
    return val;
  }

  // TODO handle clamping of Tuple between two values of other types
  // (see apply_domain.hpp)
  ossia::value operator()(const Tuple& incoming, const Tuple& min)
  {
    Tuple val;
    const auto N = incoming.size();
    const auto nmin = min.size();
    if(N == nmin)
    {
      val.reserve(N);
      for(std::size_t i = 0; i < N; i++)
      {
        val.push_back(eggs::variants::apply(*this, incoming[i].v, min[i].v));
      }
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const Tuple& min)
  {
    const auto N = val.size();
    const auto nmin = min.size();
    if(N == nmin)
    {
      for(std::size_t i = 0; i < N; i++)
      {
        val[i] = eggs::variants::apply(*this, std::move(val)[i].v, min[i].v);
      }
    }
    return std::move(val);
  }

  ossia::value operator()(const Tuple& incoming, const ossia::value& min)
  {
    Tuple val;
    const auto N = incoming.size();
    val.reserve(N);
    for(std::size_t i = 0; i < N; i++)
    {
      val.push_back(eggs::variants::apply(*this, incoming[i].v, min.v));
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const ossia::value& min)
  {
    const auto N = val.size();
    for(std::size_t i = 0; i < N; i++)
    {
      val[i] = eggs::variants::apply(*this, std::move(val)[i].v, min.v);
    }
    return std::move(val);
  }

  ossia::value operator()(const ossia::value& val, const ossia::value& min)
  {
    return eggs::variants::apply(*this, val.v, min.v);
  }

  ossia::value operator()(ossia::value&& val, const ossia::value& min)
  {
    return eggs::variants::apply(*this, std::move(val.v), min.v);
  }
};

}
