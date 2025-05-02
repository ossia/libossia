#pragma once
#include <cassert>
#include <cinttypes>
#include <optional>
#include <type_traits>
#include <utility>

namespace ossia
{
/**
 * @brief A most simple pair type, to reduce binary bloat
 */
template <typename T1, typename T2>
struct pair
{
  T1 key;
  T2 value;
};

/**
 * @brief A container useful for mapping contiguous enums to strings without
 * using too much memory
 */
template <typename K, typename V, std::size_t N>
class enum_map
{
  static_assert(std::is_enum_v<K>);

public:
  constexpr enum_map() = default;
  constexpr enum_map(const enum_map&) = default;
  constexpr enum_map(enum_map&&) noexcept = default;
  constexpr enum_map& operator=(const enum_map&) = default;
  constexpr enum_map& operator=(enum_map&&) noexcept = default;

  constexpr explicit enum_map(std::initializer_list<pair<K, V>> i)
  {
    for(auto& [k, v] : i)
    {
      auto idx = static_cast<std::underlying_type_t<K>>(k);
      assert(idx >= 0 && idx < int64_t(N));

      m_map[idx] = std::move(v);
    }
  }

  constexpr const V& operator[](K k) const noexcept
  {
    auto idx = static_cast<std::underlying_type_t<K>>(k);
    assert(idx >= 0 && idx < int64_t(N));

    return m_map[idx];
  }

  constexpr const V& operator[](int64_t idx) const noexcept
  {
    assert(idx >= 0 && idx < int64_t(N));

    return m_map[idx];
  }

  constexpr K key(const V& v) const noexcept
  {
    for(std::size_t i = 0; i < N; i++)
    {
      if(m_map[i] == v)
      {
        return static_cast<K>(i);
      }
    }

    assert(false); // key not found, hard error
    return {};
  }

  constexpr std::optional<K> find_key(const V& v) const noexcept
  {
    for(std::size_t i = 0; i < N; i++)
    {
      if(m_map[i] == v)
      {
        return static_cast<K>(i);
      }
    }

    return {};
  }

  pair<K, const V&> get(int64_t i) const noexcept
  {
    assert(i >= 0 && i < int64_t(N));
    return {static_cast<K>(i), m_map[i]};
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return N; }

private:
  V m_map[N];
};
}
