
#include <ossia/network/sockets/encoding.hpp>

#include "include_catch.hpp"

#include <cstring>
#include <string>
#include <vector>

// Helper: encode via buffer API and return as string
static std::string enc(ossia::net::encoding e, const std::string& input)
{
  auto max_sz = ossia::net::max_encoded_size(e, input.size());
  std::vector<char> buf(max_sz);
  auto actual = ossia::net::encode_to(e, input.data(), input.size(), buf.data());
  return {buf.data(), actual};
}

// Helper: decode via buffer API and return as string
static std::string dec(ossia::net::encoding e, const std::string& input)
{
  auto max_sz = ossia::net::max_decoded_size(e, input.size());
  std::vector<char> buf(max_sz);
  auto actual = ossia::net::decode_to(e, input.data(), input.size(), buf.data());
  return {buf.data(), actual};
}

// Helper: round-trip encode then decode
static std::string roundtrip(ossia::net::encoding e, const std::string& input)
{
  return dec(e, enc(e, input));
}

// ---- Base64 ----

TEST_CASE("base64_encode_basic", "base64_encode_basic")
{
  using E = ossia::net::encoding;
  // RFC 4648 test vectors
  REQUIRE(enc(E::base64, "") == "");
  REQUIRE(enc(E::base64, "f") == "Zg==");
  REQUIRE(enc(E::base64, "fo") == "Zm8=");
  REQUIRE(enc(E::base64, "foo") == "Zm9v");
  REQUIRE(enc(E::base64, "foob") == "Zm9vYg==");
  REQUIRE(enc(E::base64, "fooba") == "Zm9vYmE=");
  REQUIRE(enc(E::base64, "foobar") == "Zm9vYmFy");
}

TEST_CASE("base64_decode_basic", "base64_decode_basic")
{
  using E = ossia::net::encoding;
  REQUIRE(dec(E::base64, "") == "");
  REQUIRE(dec(E::base64, "Zg==") == "f");
  REQUIRE(dec(E::base64, "Zm8=") == "fo");
  REQUIRE(dec(E::base64, "Zm9v") == "foo");
  REQUIRE(dec(E::base64, "Zm9vYg==") == "foob");
  REQUIRE(dec(E::base64, "Zm9vYmE=") == "fooba");
  REQUIRE(dec(E::base64, "Zm9vYmFy") == "foobar");
}

TEST_CASE("base64_decode_ignores_whitespace", "base64_decode_ignores_whitespace")
{
  using E = ossia::net::encoding;
  // Whitespace and padding should be skipped
  REQUIRE(dec(E::base64, "Zm9v\nYmFy\n") == "foobar");
  REQUIRE(dec(E::base64, "Zm9v YmFy") == "foobar");
}

TEST_CASE("base64_roundtrip", "base64_roundtrip")
{
  using E = ossia::net::encoding;
  REQUIRE(roundtrip(E::base64, "Hello, World!") == "Hello, World!");
  REQUIRE(roundtrip(E::base64, "a") == "a");
  REQUIRE(roundtrip(E::base64, "ab") == "ab");
  REQUIRE(roundtrip(E::base64, "abc") == "abc");
  REQUIRE(roundtrip(E::base64, "abcd") == "abcd");
}

TEST_CASE("base64_roundtrip_binary", "base64_roundtrip_binary")
{
  using E = ossia::net::encoding;
  // Binary data with null bytes and high bytes
  std::string bin = {'\x00', '\x01', '\x02', '\xFE', '\xFF'};
  REQUIRE(roundtrip(E::base64, bin) == bin);
}

// ---- Hex ----

TEST_CASE("hex_encode_basic", "hex_encode_basic")
{
  using E = ossia::net::encoding;
  REQUIRE(enc(E::hex, "") == "");
  REQUIRE(enc(E::hex, "Hello") == "48656C6C6F");
  REQUIRE(enc(E::hex, std::string("\x00\xFF", 2)) == "00FF");
}

TEST_CASE("hex_decode_basic", "hex_decode_basic")
{
  using E = ossia::net::encoding;
  REQUIRE(dec(E::hex, "") == "");
  REQUIRE(dec(E::hex, "48656C6C6F") == "Hello");
  REQUIRE(dec(E::hex, "00FF") == std::string("\x00\xFF", 2));
  // Lowercase input
  REQUIRE(dec(E::hex, "48656c6c6f") == "Hello");
}

TEST_CASE("hex_decode_with_separators", "hex_decode_with_separators")
{
  using E = ossia::net::encoding;
  // Common hex formatting with spaces, colons, dashes
  REQUIRE(dec(E::hex, "48 65 6C 6C 6F") == "Hello");
  REQUIRE(dec(E::hex, "48:65:6C:6C:6F") == "Hello");
  REQUIRE(dec(E::hex, "48-65-6C-6C-6F") == "Hello");
}

TEST_CASE("hex_roundtrip", "hex_roundtrip")
{
  using E = ossia::net::encoding;
  REQUIRE(roundtrip(E::hex, "Hello, World!") == "Hello, World!");

  std::string bin = {'\x00', '\x80', '\xFF', '\x01'};
  REQUIRE(roundtrip(E::hex, bin) == bin);
}

// ---- Ascii85 ----

TEST_CASE("ascii85_encode_basic", "ascii85_encode_basic")
{
  using E = ossia::net::encoding;
  REQUIRE(enc(E::ascii85, "") == "");
  // All-zero block compresses to 'z'
  REQUIRE(enc(E::ascii85, std::string(4, '\0')) == "z");
}

TEST_CASE("ascii85_roundtrip", "ascii85_roundtrip")
{
  using E = ossia::net::encoding;
  // Exact 4-byte multiple
  REQUIRE(roundtrip(E::ascii85, "test") == "test");
  REQUIRE(roundtrip(E::ascii85, "testtest") == "testtest");
  // Non-multiple lengths
  REQUIRE(roundtrip(E::ascii85, "Hello") == "Hello");
  REQUIRE(roundtrip(E::ascii85, "Hi") == "Hi");
  REQUIRE(roundtrip(E::ascii85, "A") == "A");
  // All zeros
  REQUIRE(roundtrip(E::ascii85, std::string(4, '\0')) == std::string(4, '\0'));
  REQUIRE(roundtrip(E::ascii85, std::string(8, '\0')) == std::string(8, '\0'));
}

TEST_CASE("ascii85_roundtrip_binary", "ascii85_roundtrip_binary")
{
  using E = ossia::net::encoding;
  std::string bin = {'\x00', '\x01', '\x80', '\xFF', '\xFE', '\x7F', '\x42', '\x00'};
  REQUIRE(roundtrip(E::ascii85, bin) == bin);
}

// ---- Intel HEX ----

TEST_CASE("intel_hex_encode_basic", "intel_hex_encode_basic")
{
  using E = ossia::net::encoding;
  auto encoded = enc(E::intel_hex, "Hi");
  // Should start with ':' and end with EOF record
  REQUIRE(encoded.front() == ':');
  REQUIRE(encoded.find(":00000001FF\r\n") != std::string::npos);
}

TEST_CASE("intel_hex_decode_basic", "intel_hex_decode_basic")
{
  using E = ossia::net::encoding;
  // Standard Intel HEX data record: 2 bytes at address 0000
  REQUIRE(dec(E::intel_hex, ":0200000048691F\r\n:00000001FF\r\n") == "Hi");
}

TEST_CASE("intel_hex_decode_stops_at_eof", "intel_hex_decode_stops_at_eof")
{
  using E = ossia::net::encoding;
  // Data after EOF record should be ignored
  auto result = dec(E::intel_hex,
      ":02000000414289\r\n"
      ":00000001FF\r\n"
      ":020000004344XX\r\n");
  REQUIRE(result == "AB");
}

TEST_CASE("intel_hex_roundtrip", "intel_hex_roundtrip")
{
  using E = ossia::net::encoding;
  REQUIRE(roundtrip(E::intel_hex, "Hello, World!") == "Hello, World!");
  // Longer than 16 bytes (produces multiple records)
  std::string long_data = "This is a longer string that exceeds sixteen bytes by a lot!";
  REQUIRE(roundtrip(E::intel_hex, long_data) == long_data);
}

TEST_CASE("intel_hex_roundtrip_binary", "intel_hex_roundtrip_binary")
{
  using E = ossia::net::encoding;
  std::string bin = {'\x00', '\xFF', '\x80', '\x01', '\x7F'};
  REQUIRE(roundtrip(E::intel_hex, bin) == bin);
}

// ---- Motorola S-Record ----

TEST_CASE("srec_encode_basic", "srec_encode_basic")
{
  using E = ossia::net::encoding;
  auto encoded = enc(E::srec, "Hi");
  // Should start with S1 data record and end with S9 EOF
  REQUIRE(encoded.substr(0, 2) == "S1");
  REQUIRE(encoded.find("S9030000FC\r\n") != std::string::npos);
}

TEST_CASE("srec_decode_basic", "srec_decode_basic")
{
  using E = ossia::net::encoding;
  // S1 record: byte count=05, addr=0000, data=4869 ("Hi"), checksum
  REQUIRE(dec(E::srec, "S10500004869E9\r\nS9030000FC\r\n") == "Hi");
}

TEST_CASE("srec_decode_stops_at_eof", "srec_decode_stops_at_eof")
{
  using E = ossia::net::encoding;
  auto result = dec(E::srec,
      "S1050000414286\r\n"
      "S9030000FC\r\n"
      "S1050000434484\r\n");
  REQUIRE(result == "AB");
}

TEST_CASE("srec_roundtrip", "srec_roundtrip")
{
  using E = ossia::net::encoding;
  REQUIRE(roundtrip(E::srec, "Hello, World!") == "Hello, World!");
  std::string long_data = "This is a longer string that exceeds sixteen bytes by a lot!";
  REQUIRE(roundtrip(E::srec, long_data) == long_data);
}

TEST_CASE("srec_roundtrip_binary", "srec_roundtrip_binary")
{
  using E = ossia::net::encoding;
  std::string bin = {'\x00', '\xFF', '\x80', '\x01', '\x7F'};
  REQUIRE(roundtrip(E::srec, bin) == bin);
}

// ---- Cross-encoding: none ----

TEST_CASE("encoding_none_passthrough", "encoding_none_passthrough")
{
  using E = ossia::net::encoding;
  std::string data = "hello";
  // encode_to with none should be identity
  char buf[16];
  auto sz = ossia::net::encode_to(E::none, data.data(), data.size(), buf);
  REQUIRE(sz == 5);
  REQUIRE(std::string(buf, sz) == data);

  sz = ossia::net::decode_to(E::none, data.data(), data.size(), buf);
  REQUIRE(sz == 5);
  REQUIRE(std::string(buf, sz) == data);
}

// ---- Size estimation ----

TEST_CASE("max_encoded_size_sufficient", "max_encoded_size_sufficient")
{
  using E = ossia::net::encoding;
  // Verify that max_encoded_size is always >= actual encoded size
  std::string data = "The quick brown fox jumps over the lazy dog. 0123456789!";
  for(auto e : {E::base64, E::ascii85, E::hex, E::intel_hex, E::srec})
  {
    auto max_sz = ossia::net::max_encoded_size(e, data.size());
    auto encoded = ossia::net::encode_bytes(e, data.data(), data.size());
    REQUIRE(max_sz >= encoded.size());
  }
}

TEST_CASE("max_decoded_size_sufficient", "max_decoded_size_sufficient")
{
  using E = ossia::net::encoding;
  std::string data = "The quick brown fox jumps over the lazy dog. 0123456789!";
  for(auto e : {E::base64, E::ascii85, E::hex, E::intel_hex, E::srec})
  {
    auto encoded = ossia::net::encode_bytes(e, data.data(), data.size());
    auto max_sz = ossia::net::max_decoded_size(e, encoded.size());
    auto decoded = ossia::net::decode_bytes(e, encoded.data(), encoded.size());
    REQUIRE(max_sz >= decoded.size());
  }
}

// ---- Vector convenience API ----

TEST_CASE("encode_bytes_decode_bytes", "encode_bytes_decode_bytes")
{
  using E = ossia::net::encoding;
  std::string data = "convenience API test";
  for(auto e : {E::base64, E::ascii85, E::hex, E::intel_hex, E::srec})
  {
    auto encoded = ossia::net::encode_bytes(e, data.data(), data.size());
    auto decoded = ossia::net::decode_bytes(e, encoded.data(), encoded.size());
    REQUIRE(std::string(decoded.data(), decoded.size()) == data);
  }
}

// ---- Edge cases ----

TEST_CASE("encoding_empty_input", "encoding_empty_input")
{
  using E = ossia::net::encoding;
  for(auto e : {E::base64, E::hex, E::ascii85})
  {
    REQUIRE(enc(e, "") == "");
    REQUIRE(dec(e, "") == "");
  }
  // Intel HEX and SREC produce EOF records even for empty input
  REQUIRE(enc(E::intel_hex, "").find(":00000001FF") != std::string::npos);
  REQUIRE(enc(E::srec, "").find("S9030000FC") != std::string::npos);
  // Decoding an EOF-only record should yield empty data
  REQUIRE(dec(E::intel_hex, ":00000001FF\r\n") == "");
  REQUIRE(dec(E::srec, "S9030000FC\r\n") == "");
}

TEST_CASE("encoding_single_byte", "encoding_single_byte")
{
  using E = ossia::net::encoding;
  for(auto e : {E::base64, E::ascii85, E::hex, E::intel_hex, E::srec})
  {
    REQUIRE(roundtrip(e, "X") == "X");
    REQUIRE(roundtrip(e, std::string(1, '\x00')) == std::string(1, '\x00'));
    REQUIRE(roundtrip(e, std::string(1, '\xFF')) == std::string(1, '\xFF'));
  }
}
