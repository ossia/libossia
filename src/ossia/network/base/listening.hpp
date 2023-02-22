#pragma once
#include <ossia/detail/mutex.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/string_algorithms.hpp>
#include <ossia/detail/string_map.hpp>

namespace ossia
{
// MOVEME
template <typename T>
struct locked_map
{
public:
  using map_type = T;
  using key_type = typename map_type::key_type;
  using mapped_type = typename map_type::mapped_type;
  using value_type = typename map_type::value_type;

  template <typename Key>
  std::optional<mapped_type> find(const Key& path) const
  {
    lock_t lock(m_mutex);
    auto it = m_map.find(path);
    if(it != m_map.end())
    {
      return it->second;
    }
    else
    {
      return std::nullopt;
    }
  }

  template <typename Key>
  std::optional<mapped_type> find_and_take(const Key& path)
  {
    lock_t lock(m_mutex);
    auto it = m_map.find(path);
    if(it != m_map.end())
    {
      auto val = std::move(it.value());
      m_map.erase(it);
      return std::move(val);
    }
    else
    {
      return std::nullopt;
    }
  }

  void rename(const key_type& oldk, const key_type& newk)
  {
    lock_t lock(m_mutex);

    ossia::small_vector<std::pair<key_type, mapped_type>, 8> cache;

    for(auto& [k, v] : m_map)
    {
      if(string_starts_with(k, oldk))
        cache.emplace_back(k, std::move(v));
    }

    if(!cache.empty())
    {
      for(auto&& e : std::move(cache))
      {
        m_map.erase(e.first);
        e.first.replace(0, oldk.length(), newk);
        m_map.insert({std::move(e.first), std::move(e.second)});
      }
    }
  }

  void insert(const value_type& m)
  {
    lock_t lock(m_mutex);
    m_map.insert(m);
  }

  void insert(value_type&& m)
  {
    lock_t lock(m_mutex);
    m_map.insert(std::move(m));
  }

  void erase(const key_type& m)
  {
    lock_t lock(m_mutex);
    m_map.erase(m);
  }

private:
  mutable mutex_t m_mutex;
  map_type m_map TS_GUARDED_BY(m_mutex);
};

namespace net
{
class parameter_base;
using listened_parameters = locked_map<string_map<ossia::net::parameter_base*>>;
}
}
