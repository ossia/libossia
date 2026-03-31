#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace ossia::net
{
enum class encoding : uint8_t
{
  none,
  base64,
  ascii85,
  hex,
  intel_hex,
  srec
};

// Upper-bound output sizes for pre-allocation
inline std::size_t max_encoded_size(encoding enc, std::size_t sz)
{
  switch(enc)
  {
    case encoding::base64:
      return 4 * ((sz + 2) / 3);
    case encoding::ascii85:
      return sz * 5 / 4 + 5;
    case encoding::hex:
      return sz * 2;
    case encoding::intel_hex:
      // per 16-byte line: ':' + LL(2) + AAAA(4) + TT(2) + data(32) + CC(2) + \r\n = 45
      return ((sz + 15) / 16) * 45 + 13;
    case encoding::srec:
      // per 16-byte line: S1(2) + LL(2) + AAAA(4) + data(32) + CC(2) + \r\n = 44
      return ((sz + 15) / 16) * 44 + 12;
    default:
      return sz;
  }
}

inline std::size_t max_decoded_size(encoding enc, std::size_t sz)
{
  switch(enc)
  {
    case encoding::base64:
      return 3 * sz / 4 + 3;
    case encoding::ascii85:
      return sz * 4 / 5 + 4;
    case encoding::hex:
    case encoding::intel_hex:
    case encoding::srec:
      return sz / 2 + 1;
    default:
      return sz;
  }
}

namespace detail
{

inline uint8_t hex_val(char c)
{
  if(c >= '0' && c <= '9')
    return c - '0';
  if(c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if(c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return 0;
}

inline uint8_t hex_pair(const char* p) { return (hex_val(p[0]) << 4) | hex_val(p[1]); }

inline char* hex_push(char* p, uint8_t b)
{
  static constexpr char H[] = "0123456789ABCDEF";
  *p++ = H[b >> 4];
  *p++ = H[b & 0x0F];
  return p;
}

// --- Base64 (RFC 4648) ---

inline std::size_t base64_encode(const char* data, std::size_t sz, char* out)
{
  static constexpr char T[]
      = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  char* p = out;
  for(std::size_t i = 0; i < sz; i += 3)
  {
    uint32_t n = uint8_t(data[i]) << 16;
    if(i + 1 < sz)
      n |= uint8_t(data[i + 1]) << 8;
    if(i + 2 < sz)
      n |= uint8_t(data[i + 2]);
    *p++ = T[(n >> 18) & 0x3F];
    *p++ = T[(n >> 12) & 0x3F];
    *p++ = i + 1 < sz ? T[(n >> 6) & 0x3F] : '=';
    *p++ = i + 2 < sz ? T[n & 0x3F] : '=';
  }
  return std::size_t(p - out);
}

inline std::size_t base64_decode(const char* data, std::size_t sz, char* out)
{
  auto val = [](uint8_t c) -> uint8_t {
    if(c >= 'A' && c <= 'Z')
      return c - 'A';
    if(c >= 'a' && c <= 'z')
      return c - 'a' + 26;
    if(c >= '0' && c <= '9')
      return c - '0' + 52;
    if(c == '+')
      return 62;
    if(c == '/')
      return 63;
    return 0xFF;
  };
  char* p = out;
  uint32_t buf = 0;
  int bits = 0;
  for(std::size_t i = 0; i < sz; i++)
  {
    uint8_t v = val(uint8_t(data[i]));
    if(v == 0xFF)
      continue;
    buf = (buf << 6) | v;
    bits += 6;
    if(bits >= 8)
    {
      bits -= 8;
      *p++ = char((buf >> bits) & 0xFF);
    }
  }
  return std::size_t(p - out);
}

// --- Ascii85 ---

inline std::size_t ascii85_encode(const char* data, std::size_t sz, char* out)
{
  char* p = out;
  std::size_t i = 0;
  for(; i + 3 < sz; i += 4)
  {
    uint32_t n = (uint8_t(data[i]) << 24) | (uint8_t(data[i + 1]) << 16)
                 | (uint8_t(data[i + 2]) << 8) | uint8_t(data[i + 3]);
    if(n == 0)
    {
      *p++ = 'z';
    }
    else
    {
      char c[5];
      for(int j = 4; j >= 0; j--)
      {
        c[j] = char('!' + n % 85);
        n /= 85;
      }
      std::memcpy(p, c, 5);
      p += 5;
    }
  }
  if(auto rem = sz - i; rem > 0)
  {
    uint32_t n = 0;
    for(std::size_t j = 0; j < rem; j++)
      n |= uint8_t(data[i + j]) << (24 - 8 * j);
    char c[5];
    for(int j = 4; j >= 0; j--)
    {
      c[j] = char('!' + n % 85);
      n /= 85;
    }
    std::memcpy(p, c, rem + 1);
    p += rem + 1;
  }
  return std::size_t(p - out);
}

inline std::size_t ascii85_decode(const char* data, std::size_t sz, char* out)
{
  char* p = out;
  std::size_t i = 0;
  while(i < sz)
  {
    if(data[i] == 'z')
    {
      *p++ = 0;
      *p++ = 0;
      *p++ = 0;
      *p++ = 0;
      i++;
      continue;
    }
    uint32_t n = 0;
    int count = 0;
    while(i < sz && count < 5 && data[i] != 'z')
    {
      if(data[i] >= '!' && data[i] <= 'u')
      {
        n = n * 85 + uint32_t(data[i] - '!');
        count++;
      }
      i++;
    }
    if(count == 5)
    {
      *p++ = char((n >> 24) & 0xFF);
      *p++ = char((n >> 16) & 0xFF);
      *p++ = char((n >> 8) & 0xFF);
      *p++ = char(n & 0xFF);
    }
    else if(count > 1)
    {
      for(int j = count; j < 5; j++)
        n = n * 85 + 84;
      for(int j = 0; j < count - 1; j++)
        *p++ = char((n >> (24 - 8 * j)) & 0xFF);
    }
  }
  return std::size_t(p - out);
}

// --- Hex string ---

inline std::size_t hex_encode(const char* data, std::size_t sz, char* out)
{
  char* p = out;
  for(std::size_t i = 0; i < sz; i++)
    p = hex_push(p, uint8_t(data[i]));
  return std::size_t(p - out);
}

inline std::size_t hex_decode(const char* data, std::size_t sz, char* out)
{
  char* p = out;
  std::size_t i = 0;
  while(i + 1 < sz)
  {
    while(i < sz
          && (data[i] == ' ' || data[i] == ':' || data[i] == '-' || data[i] == '\r'
              || data[i] == '\n'))
      i++;
    if(i + 1 >= sz)
      break;
    *p++ = char(hex_pair(data + i));
    i += 2;
  }
  return std::size_t(p - out);
}

// --- Intel HEX ---

inline std::size_t ihex_encode(const char* data, std::size_t sz, char* out)
{
  char* p = out;
  uint16_t addr = 0;
  std::size_t i = 0;
  while(i < sz)
  {
    auto chunk = std::min<std::size_t>(sz - i, 16);
    uint8_t sum = 0;
    *p++ = ':';
    auto bc = uint8_t(chunk);
    p = hex_push(p, bc);
    sum += bc;
    p = hex_push(p, uint8_t(addr >> 8));
    sum += uint8_t(addr >> 8);
    p = hex_push(p, uint8_t(addr));
    sum += uint8_t(addr);
    p = hex_push(p, 0x00);
    for(std::size_t j = 0; j < chunk; j++)
    {
      auto b = uint8_t(data[i + j]);
      p = hex_push(p, b);
      sum += b;
    }
    p = hex_push(p, uint8_t(-sum));
    *p++ = '\r';
    *p++ = '\n';
    i += chunk;
    addr += uint16_t(chunk);
  }
  static constexpr char eof[] = ":00000001FF\r\n";
  std::memcpy(p, eof, 13);
  p += 13;
  return std::size_t(p - out);
}

inline std::size_t ihex_decode(const char* data, std::size_t sz, char* out)
{
  char* op = out;
  const char* p = data;
  const char* end = data + sz;
  while(p < end)
  {
    while(p < end && *p != ':')
      p++;
    if(p >= end)
      break;
    p++;
    if(p + 8 > end)
      break;
    auto bc = hex_pair(p);
    p += 2;
    p += 4; // address
    auto type = hex_pair(p);
    p += 2;
    if(type == 0x01)
      break;
    if(type == 0x00 && p + bc * 2 <= end)
    {
      for(int j = 0; j < bc; j++)
      {
        *op++ = char(hex_pair(p));
        p += 2;
      }
    }
    else
    {
      p += bc * 2;
    }
    if(p + 2 <= end)
      p += 2; // checksum
    while(p < end && (*p == '\r' || *p == '\n'))
      p++;
  }
  return std::size_t(op - out);
}

// --- Motorola S-Record ---

inline std::size_t srec_encode(const char* data, std::size_t sz, char* out)
{
  char* p = out;
  uint16_t addr = 0;
  std::size_t i = 0;
  while(i < sz)
  {
    auto chunk = std::min<std::size_t>(sz - i, 16);
    uint8_t sum = 0;
    *p++ = 'S';
    *p++ = '1';
    auto bc = uint8_t(2 + chunk + 1); // address(2) + data + checksum(1)
    p = hex_push(p, bc);
    sum += bc;
    p = hex_push(p, uint8_t(addr >> 8));
    sum += uint8_t(addr >> 8);
    p = hex_push(p, uint8_t(addr));
    sum += uint8_t(addr);
    for(std::size_t j = 0; j < chunk; j++)
    {
      auto b = uint8_t(data[i + j]);
      p = hex_push(p, b);
      sum += b;
    }
    p = hex_push(p, uint8_t(~sum));
    *p++ = '\r';
    *p++ = '\n';
    i += chunk;
    addr += uint16_t(chunk);
  }
  static constexpr char eof[] = "S9030000FC\r\n";
  std::memcpy(p, eof, 12);
  p += 12;
  return std::size_t(p - out);
}

inline std::size_t srec_decode(const char* data, std::size_t sz, char* out)
{
  char* op = out;
  const char* p = data;
  const char* end = data + sz;
  while(p < end)
  {
    while(p < end && *p != 'S')
      p++;
    if(p + 1 >= end)
      break;
    p++;
    char type = *p;
    p++;
    if(type == '9' || type == '8' || type == '7')
      break;
    if(p + 2 > end)
      break;
    auto bc = hex_pair(p);
    p += 2;
    int ab = (type == '0' || type == '1') ? 2
             : (type == '2')              ? 3
             : (type == '3')              ? 4
                                          : 2;
    if(p + ab * 2 > end)
      break;
    p += ab * 2;
    int db = bc - ab - 1;
    if(type >= '1' && type <= '3' && db > 0 && p + db * 2 <= end)
    {
      for(int j = 0; j < db; j++)
      {
        *op++ = char(hex_pair(p));
        p += 2;
      }
    }
    else if(db > 0)
    {
      p += db * 2;
    }
    if(p + 2 <= end)
      p += 2; // checksum
    while(p < end && (*p == '\r' || *p == '\n'))
      p++;
  }
  return std::size_t(op - out);
}

} // namespace detail

// Write encoded data directly into caller-provided buffer.
// Buffer must have at least max_encoded_size() bytes.
// Returns actual bytes written.
inline std::size_t
encode_to(encoding enc, const char* data, std::size_t sz, char* out)
{
  switch(enc)
  {
    case encoding::base64:
      return detail::base64_encode(data, sz, out);
    case encoding::ascii85:
      return detail::ascii85_encode(data, sz, out);
    case encoding::hex:
      return detail::hex_encode(data, sz, out);
    case encoding::intel_hex:
      return detail::ihex_encode(data, sz, out);
    case encoding::srec:
      return detail::srec_encode(data, sz, out);
    default:
      std::memcpy(out, data, sz);
      return sz;
  }
}

// Write decoded data directly into caller-provided buffer.
// Buffer must have at least max_decoded_size() bytes.
// Returns actual bytes written.
inline std::size_t
decode_to(encoding enc, const char* data, std::size_t sz, char* out)
{
  switch(enc)
  {
    case encoding::base64:
      return detail::base64_decode(data, sz, out);
    case encoding::ascii85:
      return detail::ascii85_decode(data, sz, out);
    case encoding::hex:
      return detail::hex_decode(data, sz, out);
    case encoding::intel_hex:
      return detail::ihex_decode(data, sz, out);
    case encoding::srec:
      return detail::srec_decode(data, sz, out);
    default:
      std::memcpy(out, data, sz);
      return sz;
  }
}

// Convenience: allocating versions
inline std::vector<char> encode_bytes(encoding enc, const char* data, std::size_t sz)
{
  if(enc == encoding::none)
    return {data, data + sz};
  std::vector<char> out(max_encoded_size(enc, sz));
  auto actual = encode_to(enc, data, sz, out.data());
  out.resize(actual);
  return out;
}

inline std::vector<char> decode_bytes(encoding enc, const char* data, std::size_t sz)
{
  if(enc == encoding::none)
    return {data, data + sz};
  std::vector<char> out(max_decoded_size(enc, sz));
  auto actual = decode_to(enc, data, sz, out.data());
  out.resize(actual);
  return out;
}

} // namespace ossia::net
