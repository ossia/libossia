#pragma once
#include <ossia/detail/optional.hpp>
#include <hopscotch_map.h>
#include <mutex>

namespace ossia
{
// MOVEME
template<typename T>
struct locked_map
{
public:
  using map_type = T;
  using key_type = typename map_type::key_type;
  using mapped_type = typename map_type::mapped_type;
  using value_type = typename map_type::value_type;

  optional<mapped_type> find(const key_type& path)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_map.find(path);
    if (it != m_map.end())
    {
      return it.value();
    }
    else
    {
      return ossia::none;
    }
  }

  optional<mapped_type> find_and_take(const key_type& path)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_map.find(path);
    if (it != m_map.end())
    {
      auto val = std::move(it.value());
      m_map.erase(it);
      return std::move(val);
    }
    else
    {
      return ossia::none;
    }
  }

  void insert(const value_type& m)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_map.insert(m);
  }

  void insert(value_type&& m)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_map.insert(std::move(m));
  }

  void erase(const key_type& m)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_map.erase(m);
  }

private:
  std::mutex m_mutex;
  map_type m_map;
};

namespace net
{
class address_base;
using listened_addresses = locked_map<tsl::hopscotch_map<std::string, ossia::net::address_base*>>;
}
}
