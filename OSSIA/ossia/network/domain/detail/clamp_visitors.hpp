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
  ossia::value operator()(T&& val, const U& min, const V& max)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{f(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{f(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{f(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{f(val.value, min.value, max.value)}; }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, const Vec<float, N>& min, const Vec<float, N>& max)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val.value[i] = f(val.value[i], min.value[i], max.value[i]);
    }
    return val;
  }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, Float min, Float max)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val.value[i] = f(val.value[i], min.value, max.value);
    }
    return val;
  }

  // TODO handle clamping of Tuple between two values of other types
  // (see apply_domain.hpp)
  ossia::value operator()(const Tuple& incoming, const Tuple& min, const Tuple& max)
  {
    Tuple val;
    const auto N = incoming.value.size();
    const auto nmin = min.value.size();
    const auto nmax = max.value.size();
    if(N == nmin && N == nmax)
    {
      val.value.reserve(N);
      for(std::size_t i = 0; i < N; i++)
      {
        val.value.push_back(eggs::variants::apply(*this, incoming.value[i].v, min.value[i].v, max.value[i].v));
      }
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const Tuple& min, const Tuple& max)
  {
    const auto N = val.value.size();
    const auto nmin = min.value.size();
    const auto nmax = max.value.size();
    if(N == nmin && N == nmax)
    {
      for(std::size_t i = 0; i < N; i++)
      {
        val.value[i] = eggs::variants::apply(*this, std::move(val).value[i].v, min.value[i].v, max.value[i].v);
      }
    }
    return std::move(val);
  }

  ossia::value operator()(const Tuple& incoming, const ossia::value& min, const ossia::value& max)
  {
    Tuple val;
    const auto N = incoming.value.size();
    val.value.reserve(N);
    for(std::size_t i = 0; i < N; i++)
    {
      val.value.push_back(eggs::variants::apply(*this, incoming.value[i].v, min.v, max.v));
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const ossia::value& min, const ossia::value& max)
  {
    const auto N = val.value.size();
    for(std::size_t i = 0; i < N; i++)
    {
      val.value[i] = eggs::variants::apply(*this, std::move(val).value[i].v, min.v, max.v);
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
  ossia::value operator()(T&& val, const U& min)
  { return std::forward<T>(val); }

  ossia::value operator()(Int val, Int min)
  { return Int{f(val.value, min.value)}; }
  ossia::value operator()(Float val, Float min)
  { return Float{f(val.value, min.value)}; }
  ossia::value operator()(Char val, Char min)
  { return Char{f(val.value, min.value)}; }
  ossia::value operator()(Bool val, Bool min)
  { return Bool{f(val.value, min.value)}; }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, const Vec<float, N>& min)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val.value[i] = f(val.value[i], min.value[i]);
    }
    return val;
  }

  template<std::size_t N>
  ossia::value operator()(Vec<float, N> val, Float min)
  {
    for(std::size_t i = 0; i < N; i++)
    {
      val.value[i] = f(val.value[i], min.value);
    }
    return val;
  }

  // TODO handle clamping of Tuple between two values of other types
  // (see apply_domain.hpp)
  ossia::value operator()(const Tuple& incoming, const Tuple& min)
  {
    Tuple val;
    const auto N = incoming.value.size();
    const auto nmin = min.value.size();
    if(N == nmin)
    {
      val.value.reserve(N);
      for(std::size_t i = 0; i < N; i++)
      {
        val.value.push_back(eggs::variants::apply(*this, incoming.value[i].v, min.value[i].v));
      }
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const Tuple& min)
  {
    const auto N = val.value.size();
    const auto nmin = min.value.size();
    if(N == nmin)
    {
      for(std::size_t i = 0; i < N; i++)
      {
        val.value[i] = eggs::variants::apply(*this, std::move(val).value[i].v, min.value[i].v);
      }
    }
    return std::move(val);
  }

  ossia::value operator()(const Tuple& incoming, const ossia::value& min)
  {
    Tuple val;
    const auto N = incoming.value.size();
    val.value.reserve(N);
    for(std::size_t i = 0; i < N; i++)
    {
      val.value.push_back(eggs::variants::apply(*this, incoming.value[i].v, min.v));
    }
    return val;
  }

  ossia::value operator()(Tuple&& val, const ossia::value& min)
  {
    const auto N = val.value.size();
    for(std::size_t i = 0; i < N; i++)
    {
      val.value[i] = eggs::variants::apply(*this, std::move(val).value[i].v, min.v);
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
