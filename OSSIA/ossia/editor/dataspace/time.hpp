#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>
#include <eggs/variant.hpp>

namespace ossia
{
struct time_dataspace;
struct second_u;
template<typename Impl>
struct time_unit
{
  using is_unit = std::true_type;
  using neutral_unit = second_u;
  using value_type = float;
  using concrete_type = Impl;
  using dataspace_type = time_dataspace;
};

struct second_u : public time_unit<second_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("second", "s"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }
};

struct bark_u : public time_unit<bark_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("bark"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.0 / (600. * std::sinh(self.dataspace_value / 6.));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 6. * std::asinh(1.0 / (self.dataspace_value * 600.0));
  }
};

struct bpm_u : public time_unit<bpm_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("bpm"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 60.0 / self.dataspace_value;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 60.0 / self.dataspace_value;
  }
};

struct cent_u : public time_unit<cent_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("cents"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1. / (440.0 * std::exp2((self.dataspace_value - 6900.0) / 1200.0));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 6900.0 + 1200.0 * std::log(1. / (440.0 * self.dataspace_value)) / ossia::ln_2;
  }
};

struct frequency_u : public time_unit<frequency_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("fps", "Hz", "hz", "Hertz"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.0 / self.dataspace_value;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 1.0 / self.dataspace_value;
  }
};

struct mel_u : public time_unit<mel_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("mel"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1.0 / (700.0 * (std::pow(10, self.dataspace_value / 2595.0) - 1.0));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 2595.0 * std::log10(1 + 1.0 / (self.dataspace_value * 700.0));
  }
};

struct midi_pitch_u : public time_unit<midi_pitch_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("midinote"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 1. / (440.0 * std::exp2((self.dataspace_value - 69.0) / 12.0 ));
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 69.0 - 12.0 * std::log(440.0 * self.dataspace_value) / ossia::ln_2;
  }
};

struct millisecond_u : public time_unit<millisecond_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("ms", "millisecond"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return 0.001 * self.dataspace_value;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 1000.0 * self.dataspace_value;
  }
};

/*
template<int N>
struct sample_u : public time_unit<sample_u<N>>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("sample"); }

  static OSSIA_DECL_RELAXED_CONSTEXPR strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self.value.value / N;
  }

  static OSSIA_DECL_RELAXED_CONSTEXPR value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.value.value * N;
  }
};
*/

static const double exp_69_12 = std::exp2(69. / 12.);
struct playback_speed_u : public time_unit<playback_speed_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("speed"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return exp_69_12 / (440.0 * self.dataspace_value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return exp_69_12 / (440.0 * self.dataspace_value);
  }
};



using second = strong_value<second_u>;
using bark = strong_value<bark_u>;
using bpm = strong_value<bpm_u>;
using cent = strong_value<cent_u>;
using frequency = strong_value<frequency_u>;
using mel = strong_value<mel_u>;
using midi_pitch = strong_value<midi_pitch_u>;
using millisecond = strong_value<millisecond_u>;
using playback_speed = strong_value<playback_speed_u>;

// template<int N>
// using sample = strong_value<sample_u<N>>;

using time_u =
  eggs::variant<
    second_u, bark_u, bpm_u, cent_u, frequency_u, mel_u, midi_pitch_u, millisecond_u, playback_speed_u>;

template<>
struct dataspace_traits<time_u>
{
  static OSSIA_DECL_RELAXED_CONSTEXPR auto text()
  { return ossia::make_string_array("time"); }
};

}
