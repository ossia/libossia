#pragma once
#include <ossia/network/dataspace/dataspace_base.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
struct timing_u;
struct second_u;
template <typename Impl>
struct timing_unit
{
  using is_unit = std::true_type;
  using neutral_unit = second_u;
  using value_type = float;
  using concrete_type = Impl;
  using dataspace_type = timing_u;
  using is_multidimensional = std::false_type;
};

struct second_u : public timing_unit<second_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("second", "s"));
  }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }
  static ossia::domain domain() { return {}; }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct bark_u : public timing_unit<bark_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("bark")); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.0f / (600.f * std::sinh(self.dataspace_value / 6.f));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 6.f * ossia::asinh(1.0f / (self.dataspace_value * 600.0f));
  }
  static ossia::domain domain() { return {}; }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct bpm_u : public timing_unit<bpm_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("bpm")); }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return 60.0f / self.dataspace_value;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 60.0f / self.dataspace_value;
  }
  static ossia::domain_base<float> domain() { return {0.f, 240.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct cent_u : public timing_unit<cent_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("cents")); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.f / (440.0f * ossia::exp2((self.dataspace_value - 6900.0f) / 1200.0f));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 6900.0f
           + 1200.0f * std::log(1.f / (440.0f * self.dataspace_value))
                 / float(ossia::ln_2);
  }

  static ossia::domain_base<float> domain() { return {0.f, 12700.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct frequency_u : public timing_unit<frequency_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("Hz", "hz", "Hertz"));
  }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return 1.0f / self.dataspace_value;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 1.0f / self.dataspace_value;
  }

  static ossia::domain_base<float> domain() { return {0.f, 24000.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

struct mel_u : public timing_unit<mel_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("mel")); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.0f / (700.0f * (std::pow(10.f, self.dataspace_value / 2595.0f) - 1.0f));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 2595.0f * std::log10(1.f + 1.0f / (self.dataspace_value * 700.0f));
  }

  static ossia::domain_base<float> domain() { return {0.f, 4016.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

struct midi_pitch_u : public timing_unit<midi_pitch_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("midinote", "midipitch", "pitch"));
  }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.f / (440.0f * ossia::exp2((self.dataspace_value - 69.0f) / 12.0f));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 69.0f - 12.0f * std::log(440.0f * self.dataspace_value) / float(ossia::ln_2);
  }

  static ossia::domain_base<float> domain() { return {0.f, 127.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

struct millisecond_u : public timing_unit<millisecond_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("ms", "millisecond"));
  }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return 0.001f * self.dataspace_value;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 1000.0f * self.dataspace_value;
  }

  static ossia::domain domain() { return {}; }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};

struct sample_u : public timing_unit<sample_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("sample")); }
  float rate = 44100;

  void set_rate(double r) { rate = r; }

  constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self.dataspace_value / rate;
  }

  constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value * rate;
  }

  static ossia::domain_base<float> domain() { return {0.f, 44100.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

template <typename T>
// segfaults on MSVC... ossia::exp2(69. / 12.);
static constexpr T exp_69_12 = 53.817370576237730753992030478925753282562192;

struct playback_speed_u : public timing_unit<playback_speed_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("speed")); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return exp_69_12<float> / (440.0f * self.dataspace_value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return exp_69_12<float> / (440.0f * self.dataspace_value);
  }

  static ossia::domain_base<float> domain() { return {0.f, 2.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

// template<int N>
// using sample = strong_value<sample_u<N>>;
}
