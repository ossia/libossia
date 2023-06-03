#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/hash.hpp>
#include <ossia/detail/hash_map.hpp>

namespace ossia
{
static constexpr unsigned char ascii_tolower(unsigned char c) noexcept
{
  constexpr unsigned char A = 'A', Z = 'Z', a = 'a';
  if(c >= A && c <= Z)
    return c - (A - a);
  else
    return c;
}

struct ascii_tolower_less
{
  constexpr bool operator()(unsigned char c1, unsigned char c2) const noexcept
  {
    return ascii_tolower(c1) < ascii_tolower(c2);
  }
};

struct ascii_tolower_equal
{
  constexpr bool operator()(unsigned char c1, unsigned char c2) const noexcept
  {
    return ascii_tolower(c1) == ascii_tolower(c2);
  }
};

struct case_insensitive_comparator
{
  using is_transparent = std::true_type;
  bool operator()(std::string_view s1, std::string_view s2) const noexcept
  {
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(), ascii_tolower_less{});
  }
};

struct case_insensitive_hash
{
  static constexpr int buffer_size = 64;
  using is_transparent = std::true_type;
  using is_avalanching = std::true_type;

  uint64_t operator()(std::string_view s1) const noexcept
  {
    namespace wyhash = ankerl::unordered_dense::detail::wyhash;

    uint64_t res = UINT64_C(0xff51afd7ed558ccd);
    unsigned char buf[buffer_size + 1];
    const char* ptr = s1.data();
    uint64_t remaining = s1.size();

    while(remaining > 0)
    {
      if(remaining >= buffer_size)
      {
        for(int i = 0; i < buffer_size; i++)
          buf[i] = ascii_tolower(ptr[i]);
        res = wyhash::mix(res, wyhash::hash(buf, buffer_size));
        remaining -= buffer_size;
        ptr += buffer_size;
      }
      else
      {
        [[likely]];
        for(uint64_t i = 0; i < remaining; i++)
          buf[i] = ascii_tolower(ptr[i]);
        res = wyhash::mix(res, wyhash::hash(buf, remaining));
        remaining = 0;
      }
    }

    return res;
  }
};

struct case_insensitive_equal
{
  using is_transparent = std::true_type;
  constexpr bool operator()(std::string_view s1, std::string_view s2) const noexcept
  {
    return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(), ascii_tolower_equal{});
  }
};

template <typename Value>
using case_insensitive_string_map
    = ossia::hash_map<std::string, Value, case_insensitive_hash, case_insensitive_equal>;
}
