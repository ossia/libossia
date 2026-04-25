#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/math.hpp>

#include <rapidhash.h>

#include <cinttypes>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

namespace ossia
{

OSSIA_INLINE uint64_t hash_bytes(const void* data, std::size_t size) noexcept;
OSSIA_INLINE uint64_t hash_string(std::string_view sv) noexcept;
template <typename T>
OSSIA_INLINE uint64_t hash_trivial(const T& v) noexcept;

struct string_hash
{
  using is_transparent = std::true_type;
  using is_avalanching = std::true_type;
  std::size_t operator()(const std::string& s) const noexcept
  {
    return hash_string(std::string_view{s});
  }

  std::size_t operator()(std::string_view s) const noexcept
  {
    return hash_string(s);
  }

  template <std::size_t N>
  std::size_t operator()(const char (&s)[N]) const noexcept
  {
    return hash_string(std::string_view{s, N - 1});
  }
};

// https://stackoverflow.com/q/20953390/1495627
struct egur_hash
{
  using is_transparent = std::true_type;

  template <typename T>
  OSSIA_INLINE OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK std::size_t
  operator()(const T* val) const noexcept
  {
    static const constexpr std::size_t shift = constexpr_log2(1 + sizeof(T));
    return (size_t)(val) >> shift;
  }

  template <typename T>
  OSSIA_INLINE OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK std::size_t
  operator()(const std::shared_ptr<T>& val) const noexcept
  {
    static const constexpr std::size_t shift = constexpr_log2(1 + sizeof(T));
    return (size_t)(val.get()) >> shift;
  }
};

template <std::size_t ApproximateObjectSizeof>
struct unknown_pointer_hash
{
  using is_transparent = std::true_type;

  template <typename T>
  OSSIA_INLINE OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK std::size_t
  operator()(const T* val) const noexcept
  {
    static const constexpr std::size_t shift
        = constexpr_log2(1 + ApproximateObjectSizeof);
    return (size_t)(val) >> shift;
  }

  template <typename T>
  OSSIA_INLINE OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK std::size_t
  operator()(const std::shared_ptr<T>& val) const noexcept
  {
    static const constexpr std::size_t shift
        = constexpr_log2(1 + ApproximateObjectSizeof);
    return (size_t)(val.get()) >> shift;
  }
};

template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

struct pointer_hash_rapid
{
  using is_transparent = std::true_type;
  using is_avalanching = std::true_type;
  template <typename T>
  OSSIA_INLINE uint64_t operator()(const T* val) const noexcept
  {
    const uintptr_t p = reinterpret_cast<uintptr_t>(val);
    return hash_trivial(p);
  }

  template <typename T>
  OSSIA_INLINE uint64_t operator()(const std::shared_ptr<T>& val) const noexcept
  {
    const uintptr_t p = reinterpret_cast<uintptr_t>(val.get());
    return hash_trivial(p);
  }
};

namespace detail_hash
{
template <typename T>
concept has_std_hash = requires(const T& v) {
  { std::hash<T>{}(v) } -> std::convertible_to<std::size_t>;
};
template <typename T>
concept has_std_hash_avalanching
    = has_std_hash<T> && requires { typename std::hash<T>::is_avalanching; };
}
template <typename T>
struct hash
{
  OSSIA_INLINE std::size_t operator()(const T& v) const
      noexcept(noexcept(std::declval<std::hash<T>>()(std::declval<const T&>())))
  {
    static_assert(
        detail_hash::has_std_hash<T>,
        "ossia::hash<T>: T is not trivially copyable and has no "
        "std::hash<T> specialization. Specialize ossia::hash<T> "
        "(see string_hash / pointer_hash_rapid for reference) or "
        "provide std::hash<T>.");
    return std::hash<T>{}(v);
  }
};
template <typename T>
requires std::is_trivially_copyable_v<T>
      && (!std::is_pointer_v<T>)
      && (!is_shared_ptr<T>::value)
struct hash<T>
{
  using is_transparent = std::true_type;
  using is_avalanching = std::true_type;
  OSSIA_INLINE uint64_t operator()(const T& v) const noexcept
  {
    return hash_trivial(v);
  }
};

template <typename T>
requires (!std::is_trivially_copyable_v<T>)
      && (!std::is_pointer_v<T>)
      && (!is_shared_ptr<T>::value)
      && (!std::is_same_v<T, std::string>)
      && (!std::is_same_v<T, std::string_view>)
      && detail_hash::has_std_hash_avalanching<T>
struct hash<T>
{
  using is_avalanching = std::true_type;

  OSSIA_INLINE std::size_t operator()(const T& v) const
      noexcept(noexcept(std::declval<std::hash<T>>()(std::declval<const T&>())))
  {
    return std::hash<T>{}(v);
  }
};

template <typename T>
requires std::is_pointer_v<T>
struct hash<T> : pointer_hash_rapid { };

template <typename T>
requires is_shared_ptr<T>::value
struct hash<T> : pointer_hash_rapid { };

template <>
struct hash<std::string> : string_hash { };
template <>
struct hash<std::string_view> : string_hash { };

// Invalid overloads
template <>
struct hash<const char*>;
template <>
struct hash<char*>;



struct string_equal
{
  using is_transparent = std::true_type;
  bool operator()(const std::string& s, const std::string& s2) const noexcept
  {
    return s == s2;
  }
  bool operator()(std::string_view s, const std::string& s2) const noexcept
  {
    return s == s2;
  }
  bool operator()(const std::string& s, std::string_view s2) const noexcept
  {
    return s == s2;
  }
  bool operator()(std::string_view s, std::string_view s2) const noexcept
  {
    return s == s2;
  }

  template <std::size_t N>
  bool operator()(const std::string& s, const char (&s2)[N]) const noexcept
  {
    return operator()(s, std::string_view{s2, N-1});
  }

  template <std::size_t N>
  bool operator()(std::string_view s, const char (&s2)[N]) const noexcept
  {
    return operator()(s, std::string_view{s2, N-1});
  }

  template <std::size_t N>
  bool operator()(const char (&s)[N], const std::string& s2) const noexcept
  {
    return operator()(std::string_view{s, N-1}, s2);
  }

  template <std::size_t N>
  bool operator()(const char (&s)[N], std::string_view s2) const noexcept
  {
    return operator()(std::string_view{s, N-1}, s2);
  }
};


struct pointer_equal
{
  using is_transparent = std::true_type;
  template<typename U, typename V>
  OSSIA_INLINE
  bool operator()(const U* lhs, const V* rhs) const noexcept { return lhs == rhs; }

  template<typename U, typename V>
  OSSIA_INLINE
  bool operator()(const std::shared_ptr<U>& lhs, const V* rhs) const noexcept
  {
    return lhs.get() == rhs;
  }

  template<typename U, typename V>
  OSSIA_INLINE
  bool operator()(const U* lhs, const std::shared_ptr<V>& rhs) const noexcept
  {
    return lhs == rhs.get();
  }

  template<typename U, typename V>
  OSSIA_INLINE
  bool
  operator()(const std::shared_ptr<U>& lhs, const std::shared_ptr<V>& rhs) const noexcept
  {
    return lhs == rhs;
  }
};

template <typename T>
struct equal_to : std::equal_to<T> { };

template <>
struct equal_to<std::string> : string_equal { };
template <>
struct equal_to<std::string_view> : string_equal { };

template <typename T>
requires std::is_pointer_v<T>
struct equal_to<T> : pointer_equal { };

template <typename T>
requires is_shared_ptr<T>::value
struct equal_to<T> : pointer_equal { };

OSSIA_INLINE uint64_t hash_bytes(const void* data, std::size_t size) noexcept
{
  return rapidhashMicro(data, size);
}
template <typename T>
OSSIA_INLINE uint64_t hash_trivial(const T& v) noexcept
{
  static_assert(std::is_trivially_copyable_v<T>,
                "hash_trivial requires trivially copyable T");
  if constexpr(sizeof(T) <= 48)
    return rapidhashNano(&v, sizeof(T));
  else if constexpr(sizeof(T) <= 80)
    return rapidhashMicro(&v, sizeof(T));
  else
    return rapidhash(&v, sizeof(T));
}
OSSIA_INLINE uint64_t hash_string(std::string_view sv) noexcept
{
  return rapidhashMicro(sv.data(), sv.size());
}
// hash_combine_impl taken from boost
template <typename T>
OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK constexpr inline void
hash_combine(std::size_t& seed, const T& k) noexcept
{
  using namespace std;
  seed ^= hash<T>{}(k) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T>
OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK constexpr inline void
hash_combine(std::size_t& seed, const T* k) noexcept
{
  using namespace std;
  seed ^= egur_hash{}(k) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, uint8_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, int8_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, uint16_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, int16_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, uint32_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, int32_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& seed, int64_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, uint8_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, int8_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, uint16_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, int16_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, int32_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, uint64_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& seed, int64_t k) noexcept
{
  seed ^= k + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint32_t& h1, uint32_t k1) noexcept
{
  constexpr auto rotl32
      = [](uint32_t x, int8_t r) noexcept { return (x << r) | (x >> (32 - r)); };

  constexpr uint32_t c1 = 0xcc9e2d51;
  constexpr uint32_t c2 = 0x1b873593;

  k1 *= c1;
  k1 = rotl32(k1, 15);
  k1 *= c2;

  h1 ^= k1;
  h1 = rotl32(h1, 13);
  h1 = h1 * 5 + 0xe6546b64;
}

OSSIA_DISABLE_UBSAN_UNSIGNED_INTEGER_CHECK
constexpr inline void hash_combine(uint64_t& h, uint64_t k) noexcept
{
  constexpr auto m = UINT64_C(0xc6a4a7935bd1e995);
  constexpr int r = 47;

  k *= m;
  k ^= k >> r;
  k *= m;

  h ^= k;
  h *= m;

  // Completely arbitrary number, to prevent 0's
  // from hashing to 0.
  h += 0xe6546b64;
}
}
