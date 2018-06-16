#pragma once
#include <ossia/detail/math.hpp>
#include <cinttypes>
#include <cstdint>
#include <memory>

namespace ossia
{

static constexpr inline uint32_t rotl32 ( uint32_t x, int8_t r ) noexcept
{
  return (x << r) | (x >> (32 - r));
}

static constexpr inline uint64_t rotl64 ( uint64_t x, int8_t r ) noexcept
{
  return (x << r) | (x >> (64 - r));
}

// hash_combine_impl taken from boost

// https://stackoverflow.com/q/20953390/1495627
template<typename T>
struct EgurHash
{
    std::size_t operator()(const T* val) const noexcept
    {
      static const constexpr std::size_t shift = constexpr_log2(1 + sizeof(T));
      return (size_t)(val) >> shift;
    }
    std::size_t operator()(const std::shared_ptr<T>& val) const noexcept
    {
      static const constexpr std::size_t shift = constexpr_log2(1 + sizeof(T));
      return (size_t)(val.get()) >> shift;
    }
};

template<typename T>
constexpr inline void hash_combine(std::size_t& seed, const T& k) noexcept
{
  using namespace std;
  seed ^= hash<T>{}(k) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template<typename T>
constexpr inline void hash_combine(std::size_t& seed, const T* k) noexcept
{
  using namespace std;
  seed ^= EgurHash<T>{}(k) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

constexpr inline void hash_combine(uint64_t& seed, uint8_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint64_t& seed, int8_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint64_t& seed, uint16_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint64_t& seed, int16_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint64_t& seed, uint32_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint64_t& seed, int32_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint64_t& seed, int64_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }

constexpr inline void hash_combine(uint32_t& seed, uint8_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint32_t& seed, int8_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint32_t& seed, uint16_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint32_t& seed, int16_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint32_t& seed, int32_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint32_t& seed, uint64_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }
constexpr inline void hash_combine(uint32_t& seed, int64_t k) noexcept
{ seed ^= k + 0x9e3779b9 + (seed<<6) + (seed>>2); }

constexpr inline void hash_combine(uint32_t& h1, uint32_t k1) noexcept
{
  constexpr uint32_t c1 = 0xcc9e2d51;
  constexpr uint32_t c2 = 0x1b873593;

  k1 *= c1;
  k1 = rotl32(k1,15);
  k1 *= c2;

  h1 ^= k1;
  h1 = rotl32(h1,13);
  h1 = h1*5+0xe6546b64;
}

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
