#pragma once
#include <ossia/editor/dataspace/dataspace_base.hpp>

namespace ossia
{
struct linear_u;
template<typename Impl>
struct gain_unit
{
  using neutral_unit = linear_u;
  using value_type = Float;
  using concrete_type = Impl;
  using dataspace_type = struct gain_dataspace;
};

/** Utility functions taken from Jamoma TTBase.h **/
namespace detail
{
const constexpr double DecibelHeadroom = 96.;
const double GainMidiPower = std::log( std::log(12. / DecibelHeadroom + 1.) / std::log(127. / 100.) ) / std::log(2.);
const double GainMidiPowPow2 = std::pow(2., GainMidiPower);

template<typename T>
T LinearGainToDecibels(const T value)
{
    return value >= 0.
            ? 20.0 * std::log10(value)
            : 0.;
}

template<typename T>
T LinearGainToDecibelsClipped(const T value)
{
    return value <= 0.
            ? -DecibelHeadroom
            : clamp_min(20.0 * std::log10(value), -DecibelHeadroom);
}

template<typename T>
T DecibelsToLinearGain(const T value)
{
    return std::pow(10., value * 0.05);
}

template<typename T>
T DecibelsToLinearGainClipped(const T value)
{
    return value <= -DecibelHeadroom
            ? 0.
            : DecibelsToLinearGain(value);
}

template<typename T>
T MidiToLinearGain(const T value)
{
    return value <= 0.
            ? 0.
            : DecibelsToLinearGainClipped(
                  DecibelHeadroom * ( std::pow( value / 100., std::pow(2., GainMidiPower)) - 1.));
}

template<typename T>
T DecibelsToMidi(const T value)
{
    return value <= -DecibelHeadroom
            ? 0.
            : 100. * std::exp(std::log1p(value / DecibelHeadroom) / GainMidiPowPow2);
}

template<typename T>
T LinearGainToMidi(const T value)
{
    return DecibelsToMidi(LinearGainToDecibels(value));
}
}

struct linear_u : public gain_unit<linear_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("linear"); }

  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.value;
  }
};

struct midigain_u : public gain_unit<midigain_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("midigain"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
      return detail::MidiToLinearGain(self.value.value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
      return detail::LinearGainToMidi(self.value.value);
  }
};

struct decibel_u : public gain_unit<decibel_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("db", "dB"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
      return detail::DecibelsToLinearGainClipped(self.value.value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
      return detail::LinearGainToDecibelsClipped(self.value.value);
  }
};


struct decibel_raw_u : public gain_unit<decibel_raw_u>
{
  static constexpr auto text()
  { return ossia::make_string_array("db-raw", "dB-raw"); }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
      return detail::DecibelsToLinearGain(self.value.value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
      return 20.0 * (std::log10(self.value.value));
  }
};

using linear = strong_value<linear_u>;
using midigain = strong_value<midigain_u>;
using decibel = strong_value<decibel_u>;
using decibel_raw = strong_value<decibel_raw_u>;

using gain_u =
  eggs::variant<
    linear_u, midigain_u, decibel_u, decibel_raw_u>;
}
