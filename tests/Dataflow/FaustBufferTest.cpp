#include <ossia/dataflow/nodes/faust/faust_utils.hpp>

#include <include_catch.hpp>

#include <array>
#include <vector>

using ossia::nodes::faust_node_utils;

TEST_CASE("Faust float inputs use the active interval and pad missing samples", "[faust]")
{
  ossia::audio_port input;
  input.set_channels(2);
  input.channel(0) = {10., 11., 12., 13., 14., 15.};
  input.channel(1) = {20., 21., 22.};

  std::array<float, 14> storage;
  storage.fill(-1.f);
  float* channels[3];
  faust_node_utils::copy_input(2, 4, 3, storage.data() + 1, channels, input);

  REQUIRE(std::vector<float>(channels[0], channels[0] + 4)
          == std::vector<float>{12.f, 13.f, 14.f, 15.f});
  REQUIRE(std::vector<float>(channels[1], channels[1] + 4)
          == std::vector<float>{22.f, 0.f, 0.f, 0.f});
  REQUIRE(std::vector<float>(channels[2], channels[2] + 4)
          == std::vector<float>{0.f, 0.f, 0.f, 0.f});
  REQUIRE(storage.front() == -1.f);
  REQUIRE(storage.back() == -1.f);
}

TEST_CASE("Faust mono float input is silent at or beyond the input end", "[faust]")
{
  ossia::audio_channel input{10., 11., 12.};
  const auto start = GENERATE(3, 5);
  std::array<float, 4> storage{1.f, 2.f, 3.f, 4.f};

  faust_node_utils::copy_input_mono(start, storage.size(), storage.data(), input);

  REQUIRE(storage == std::array<float, 4>{0.f, 0.f, 0.f, 0.f});
}

TEST_CASE("Faust float output preserves inactive frames and declared channels", "[faust]")
{
  const auto channel_count = GENERATE(0, 1, 2);
  ossia::audio_port output;
  output.set_channels(3);
  for(int c = 0; c < 3; ++c)
  {
    output.channel(c).resize(8);
    for(int i = 0; i < 8; ++i)
      output.channel(c)[i] = 100. * (c + 1) + i;
  }
  float left[] = {3.25f, 3.5f};
  float right[] = {5.75f, 6.f};
  float* channels[] = {left, right};

  faust_node_utils::copy_output(3, 2, 8, channel_count, channels, output);

  REQUIRE(output.channels() == channel_count);
  for(int c = 0; c < channel_count; ++c)
  {
    REQUIRE(output.channel(c).size() == 8);
    for(int i = 0; i < 8; ++i)
    {
      CAPTURE(c, i);
      const double expected = i >= 3 && i < 5
                                  ? channels[c][i - 3]
                                  : 100. * (c + 1) + i;
      REQUIRE(output.channel(c)[i] == expected);
    }
  }
}

TEST_CASE("Faust float output initializes new inactive frames to silence", "[faust]")
{
  ossia::audio_port output;
  float samples[] = {3.25f, 3.5f};
  float* channels[] = {samples};

  faust_node_utils::copy_output(3, 2, 8, 1, channels, output);

  REQUIRE(output.channels() == 1);
  REQUIRE(output.channel(0).size() == 8);
  for(int i = 0; i < 8; ++i)
  {
    CAPTURE(i);
    const double expected = i >= 3 && i < 5 ? samples[i - 3] : 0.;
    REQUIRE(output.channel(0)[i] == expected);
  }
}
