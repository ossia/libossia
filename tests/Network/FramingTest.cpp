
#include <ossia/detail/pod_vector.hpp>
#include <ossia/network/sockets/cobs_framing.hpp>
#include <ossia/network/sockets/fixed_length_framing.hpp>
#include <ossia/network/sockets/line_framing.hpp>
#include <ossia/network/sockets/slip_framing.hpp>
#include <ossia/network/sockets/stx_etx_framing.hpp>
#include <ossia/network/sockets/size_prefix_framing.hpp>
#include <ossia/network/sockets/var_size_prefix_framing.hpp>

#include "include_catch.hpp"

#include <cstring>
#include <string>
#include <vector>

// Fake socket that records writes into a buffer (for encoder testing)
struct fake_socket
{
  std::vector<uint8_t> written;

  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers)
  {
    std::size_t total = 0;
    for(auto it = boost::asio::buffer_sequence_begin(buffers);
        it != boost::asio::buffer_sequence_end(buffers); ++it)
    {
      auto buf = *it;
      auto* p = static_cast<const uint8_t*>(buf.data());
      written.insert(written.end(), p, p + buf.size());
      total += buf.size();
    }
    return total;
  }

  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers, boost::system::error_code& ec)
  {
    ec = {};
    return write_some(buffers);
  }
};

namespace boost::asio
{
template <typename ConstBufferSequence>
std::size_t write(fake_socket& sock, const ConstBufferSequence& buffers)
{
  return sock.write_some(buffers);
}
}

// Helper: encode data through an encoder, return the wire bytes
template <typename Encoder>
std::vector<uint8_t> encode(const std::string& input)
{
  fake_socket sock;
  Encoder enc{sock};
  enc.write(input.data(), input.size());
  return sock.written;
}

// ---- SLIP round-trip ----

TEST_CASE("slip_encoder_basic", "slip_encoder_basic")
{
  auto wire = encode<ossia::net::slip_encoder<fake_socket>>("hello");
  // Should be: SOT(192) + "hello" + EOT(192)
  REQUIRE(wire.front() == 192);
  REQUIRE(wire.back() == 192);
  REQUIRE(wire.size() == 7); // SOT + 5 + EOT
}

TEST_CASE("slip_encoder_escaping", "slip_encoder_escaping")
{
  // Input contains the EOT byte (192) and ESC byte (219)
  std::string input = {char(192), char(219), 'A'};
  auto wire = encode<ossia::net::slip_encoder<fake_socket>>(input);
  // SOT + ESC ESC_END + ESC ESC_ESC + 'A' + EOT = 1 + 2 + 2 + 1 + 1 = 7
  REQUIRE(wire.size() == 7);
  REQUIRE(wire[0] == 192);                    // SOT
  REQUIRE(wire[1] == 219);                    // ESC
  REQUIRE(wire[2] == 220);                    // ESC_END (escaped EOT)
  REQUIRE(wire[3] == 219);                    // ESC
  REQUIRE(wire[4] == 221);                    // ESC_ESC (escaped ESC)
  REQUIRE(wire[5] == 'A');                    // normal byte
  REQUIRE(wire[6] == 192);                    // EOT
}

// ---- COBS round-trip ----

TEST_CASE("cobs_encoder_basic", "cobs_encoder_basic")
{
  auto wire = encode<ossia::net::cobs_encoder<fake_socket>>("hello");
  // Should end with 0x00 delimiter
  REQUIRE(wire.back() == 0x00);
  // "hello" has no zero bytes, so overhead block is [6, 'h','e','l','l','o'] + 0x00
  REQUIRE(wire.size() == 7); // 1 code + 5 data + 1 delimiter
  REQUIRE(wire[0] == 6);     // code byte: 5 data bytes + 1
}

TEST_CASE("cobs_encoder_with_zeros", "cobs_encoder_with_zeros")
{
  // Input: "A\0B" (3 bytes with embedded zero)
  std::string input = {'A', '\0', 'B'};
  auto wire = encode<ossia::net::cobs_encoder<fake_socket>>(input);
  // [2, 'A'] [2, 'B'] 0x00
  REQUIRE(wire.size() == 5);
  REQUIRE(wire[0] == 2);    // code: 1 data byte follows
  REQUIRE(wire[1] == 'A');
  REQUIRE(wire[2] == 2);    // code: 1 data byte follows
  REQUIRE(wire[3] == 'B');
  REQUIRE(wire[4] == 0x00); // delimiter
}

// ---- STX/ETX round-trip ----

TEST_CASE("stx_etx_encoder_basic", "stx_etx_encoder_basic")
{
  auto wire
      = encode<ossia::net::stx_etx_framing::encoder<fake_socket>>("hello");
  REQUIRE(wire.front() == 0x02); // STX
  REQUIRE(wire.back() == 0x03);  // ETX
  REQUIRE(wire.size() == 7);     // STX + 5 + ETX
}

TEST_CASE("stx_etx_encoder_escaping", "stx_etx_encoder_escaping")
{
  // Input contains STX(0x02), ETX(0x03), DLE(0x10)
  std::string input = {char(0x02), char(0x03), char(0x10), 'A'};
  auto wire
      = encode<ossia::net::stx_etx_framing::encoder<fake_socket>>(input);
  // STX + (DLE 0x02) + (DLE 0x03) + (DLE 0x10) + 'A' + ETX = 1 + 2+2+2 + 1 + 1 = 9
  REQUIRE(wire.size() == 9);
  REQUIRE(wire[0] == 0x02);  // STX
  REQUIRE(wire[1] == 0x10);  // DLE (escape)
  REQUIRE(wire[2] == 0x02);  // escaped STX
  REQUIRE(wire[3] == 0x10);  // DLE
  REQUIRE(wire[4] == 0x03);  // escaped ETX
  REQUIRE(wire[5] == 0x10);  // DLE
  REQUIRE(wire[6] == 0x10);  // escaped DLE
  REQUIRE(wire[7] == 'A');
  REQUIRE(wire[8] == 0x03);  // ETX
}

// ---- Size-prefix variants ----

TEST_CASE("size_prefix_4byte_be", "size_prefix_4byte_be")
{
  auto wire
      = encode<ossia::net::size_prefix_framing::encoder<fake_socket>>("hello");
  REQUIRE(wire.size() == 9); // 4-byte header + 5 data
  // Big-endian size = 5 → [0x00, 0x00, 0x00, 0x05]
  REQUIRE(wire[0] == 0x00);
  REQUIRE(wire[1] == 0x00);
  REQUIRE(wire[2] == 0x00);
  REQUIRE(wire[3] == 0x05);
  REQUIRE(wire[4] == 'h');
}

TEST_CASE("size_prefix_1byte", "size_prefix_1byte")
{
  auto wire
      = encode<ossia::net::size_prefix_1byte_framing::encoder<fake_socket>>(
          "hello");
  REQUIRE(wire.size() == 6); // 1-byte header + 5 data
  REQUIRE(wire[0] == 0x05);
  REQUIRE(wire[1] == 'h');
}

TEST_CASE("size_prefix_2byte_be", "size_prefix_2byte_be")
{
  auto wire
      = encode<ossia::net::size_prefix_2byte_be_framing::encoder<fake_socket>>(
          "hello");
  REQUIRE(wire.size() == 7); // 2-byte header + 5 data
  REQUIRE(wire[0] == 0x00);
  REQUIRE(wire[1] == 0x05);
  REQUIRE(wire[2] == 'h');
}

TEST_CASE("size_prefix_2byte_le", "size_prefix_2byte_le")
{
  auto wire
      = encode<ossia::net::size_prefix_2byte_le_framing::encoder<fake_socket>>(
          "hello");
  REQUIRE(wire.size() == 7); // 2-byte header + 5 data
  REQUIRE(wire[0] == 0x05);
  REQUIRE(wire[1] == 0x00);  // little-endian
  REQUIRE(wire[2] == 'h');
}

TEST_CASE("size_prefix_4byte_le", "size_prefix_4byte_le")
{
  auto wire
      = encode<ossia::net::size_prefix_4byte_le_framing::encoder<fake_socket>>(
          "hello");
  REQUIRE(wire.size() == 9);
  REQUIRE(wire[0] == 0x05);
  REQUIRE(wire[1] == 0x00);
  REQUIRE(wire[2] == 0x00);
  REQUIRE(wire[3] == 0x00); // little-endian
  REQUIRE(wire[4] == 'h');
}

// ---- Line framing ----

TEST_CASE("line_framing_encoder", "line_framing_encoder")
{
  fake_socket sock;
  ossia::net::line_framing_encoder<fake_socket> enc{sock};
  std::strcpy(enc.delimiter, "\r\n");
  enc.write("hello", 5);
  auto& wire = sock.written;
  REQUIRE(wire.size() == 7); // "hello" + "\r\n"
  REQUIRE(wire[5] == '\r');
  REQUIRE(wire[6] == '\n');
}

// ---- Fixed-length (encoder is pass-through) ----

TEST_CASE("fixed_length_encoder", "fixed_length_encoder")
{
  auto wire
      = encode<ossia::net::fixed_length_encoder<fake_socket>>("hello");
  REQUIRE(wire.size() == 5); // pass-through, no framing added
  REQUIRE(wire[0] == 'h');
}

// ---- Empty input ----

TEST_CASE("slip_empty", "slip_empty")
{
  auto wire = encode<ossia::net::slip_encoder<fake_socket>>("");
  // SOT + EOT only
  REQUIRE(wire.size() == 2);
  REQUIRE(wire[0] == 192);
  REQUIRE(wire[1] == 192);
}

TEST_CASE("cobs_empty", "cobs_empty")
{
  auto wire = encode<ossia::net::cobs_encoder<fake_socket>>("");
  // code byte (1) + delimiter (0x00)
  REQUIRE(wire.size() == 2);
  REQUIRE(wire[0] == 1);
  REQUIRE(wire[1] == 0x00);
}

TEST_CASE("stx_etx_empty", "stx_etx_empty")
{
  auto wire
      = encode<ossia::net::stx_etx_framing::encoder<fake_socket>>("");
  REQUIRE(wire.size() == 2);
  REQUIRE(wire[0] == 0x02);
  REQUIRE(wire[1] == 0x03);
}
