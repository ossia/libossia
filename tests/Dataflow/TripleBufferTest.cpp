// A value written in place into a triple buffer reaches the reader whole,
// and the producer's containers keep their capacity from one write to the next.

#include <ossia/detail/triple_buffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <string>
#include <tuple>
#include <vector>

namespace
{
using value = std::tuple<float, std::vector<float>, std::string>;

void write(ossia::triple_buffer<value>& b, float f, std::size_t n)
{
  auto& [x, v, s] = b.write_buffer();
  x = f;
  v.assign(n, f);
  s = "abc";
  b.publish();
}
}

TEST_CASE("The reader gets the latest value written in place", "[triple_buffer]")
{
  ossia::triple_buffer<value> b;
  CHECK(!b.consume());

  write(b, 1.f, 4);
  write(b, 2.f, 8);
  REQUIRE(b.consume());
  const auto& [x, v, s] = b.read_buffer();
  CHECK(x == 2.f);
  CHECK(v == std::vector<float>(8, 2.f));
  CHECK(s == "abc");

  CHECK(!b.consume());
  CHECK(std::get<0>(b.read_buffer()) == 2.f);
}

TEST_CASE("Writing in place stops allocating once every slot is warm", "[triple_buffer]")
{
  ossia::triple_buffer<value> b;
  // Each slot gets its capacity the first time the producer writes into it.
  for(int i = 0; i < 3; i++)
  {
    write(b, float(i), 64);
    b.consume();
  }

  std::vector<const float*> seen;
  for(int i = 0; i < 12; i++)
  {
    seen.push_back(std::get<1>(b.write_buffer()).data());
    write(b, float(i), 64);
    if(i % 2)
      b.consume();
  }
  std::sort(seen.begin(), seen.end());
  seen.erase(std::unique(seen.begin(), seen.end()), seen.end());
  CHECK(seen.size() <= 3);
}

TEST_CASE("A trivially copyable value is written and read in place too", "[triple_buffer]")
{
  ossia::triple_buffer<std::array<float, 4>> b;
  CHECK(!b.consume());
  b.write_buffer() = {1.f, 2.f, 3.f, 4.f};
  b.publish();
  b.write_buffer() = {5.f, 6.f, 7.f, 8.f};
  b.publish();
  REQUIRE(b.consume());
  CHECK(b.read_buffer()[0] == 5.f);
  CHECK(b.read_buffer()[3] == 8.f);
  CHECK(!b.consume());
  CHECK(b.read_buffer()[1] == 6.f);
}

TEST_CASE("A value replaces the previous one only if it was not read", "[triple_buffer]")
{
  ossia::triple_buffer<value> b;
  write(b, 1.f, 1);

  // Not read yet: replaced.
  std::get<0>(b.write_buffer()) = 2.f;
  CHECK(b.publish_if_unread());
  REQUIRE(b.consume());
  CHECK(std::get<0>(b.read_buffer()) == 2.f);

  // Read: nothing is published.
  std::get<0>(b.write_buffer()) = 3.f;
  CHECK(!b.publish_if_unread());
  CHECK(!b.consume());
}
