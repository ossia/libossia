#pragma once
#include <ossia/network/dataspace/dataspace_base.hpp>
#include <ossia/network/domain/domain_base_impl.hpp>

namespace ossia
{
struct linear_u;
struct gain_u;
template <typename Impl>
struct gain_unit
{
  using is_unit = std::true_type;
  using neutral_unit = linear_u;
  using value_type = float;
  using concrete_type = Impl;
  using dataspace_type = gain_u;
  using is_multidimensional = std::false_type;
};

/** Utility functions taken from Jamoma TTBase.h **/
namespace detail
{
template <typename T>
static const constexpr T DecibelHeadroom = 96.;
/*
template<typename T>
const T GainMidiPower
    = std::log(ossia::log1p(T(12.) / DecibelHeadroom<T>) / std::log(T(127. /
100.))) / std::log(T(2.));

template<typename T>
const T GainMidiPowPow2 = ossia::exp2(GainMidiPower<T>);
*/
template <typename T>
static const constexpr T GainMidiPower = -1.02098087016161476192621649;

template <typename T>
static const constexpr T GainMidiPowPow2 = 0.492781202603372214809951402;

template <typename T>
T LinearGainToDecibels(const T value)
{
  return value >= T(0.) ? T(20.0) * std::log10(value) : T(0.);
}

template <typename T>
T LinearGainToDecibelsClipped(const T value)
{
  return value <= T(0.) ? -DecibelHeadroom<T>
                        : ossia::max(T(20.0) * std::log10(value), -DecibelHeadroom<T>);
}

template <typename T>
T DecibelsToLinearGain(const T value)
{
  return std::pow(T(10.), value * T(0.05));
}

template <typename T>
T DecibelsToLinearGainClipped(const T value)
{
  return value <= -DecibelHeadroom<T> ? T(0.) : DecibelsToLinearGain(value);
}

template <typename T>
T MidiToLinearGain(const T value)
{
  return value <= T(0.)
             ? T(0.)
             : DecibelsToLinearGainClipped(
                 DecibelHeadroom<
                     T> * (std::pow(value / T(100.), GainMidiPowPow2<T>) - T(1.)));
}

template <typename T>
T DecibelsToMidi(const T value)
{
  return value <= -DecibelHeadroom<T>
             ? T(0.)
             : T(100.)
                   * std::exp(
                       ossia::log1p(value / DecibelHeadroom<T>) / GainMidiPowPow2<T>);
}

template <typename T>
T LinearGainToMidi(const T value)
{
  return DecibelsToMidi(LinearGainToDecibels(value));
}
}

struct OSSIA_EXPORT linear_u : public gain_unit<linear_u>
{
  static constexpr auto text() { constexpr_return(ossia::make_string_array("linear")); }

  static constexpr strong_value<neutral_unit>
  to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.dataspace_value;
  }

  static ossia::domain_base<float> domain() { return {0.f, 1.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

struct OSSIA_EXPORT midigain_u : public gain_unit<midigain_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("midigain"));
  }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return detail::MidiToLinearGain(self.dataspace_value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return detail::LinearGainToMidi(self.dataspace_value);
  }

  static ossia::domain_base<float> domain() { return {0.f, 127.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

struct OSSIA_EXPORT decibel_u : public gain_unit<decibel_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("db", "dB"));
  }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return detail::DecibelsToLinearGainClipped(self.dataspace_value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return detail::LinearGainToDecibelsClipped(self.dataspace_value);
  }

  static ossia::domain_base<float> domain() { return {-96.f, 12.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::CLAMP_LOW; }
};

struct OSSIA_EXPORT decibel_raw_u : public gain_unit<decibel_raw_u>
{
  static constexpr auto text()
  {
    constexpr_return(ossia::make_string_array("db-raw", "dB-raw"));
  }

  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return detail::DecibelsToLinearGain(self.dataspace_value);
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return 20.0f * std::log10(self.dataspace_value);
  }

  static ossia::domain_base<float> domain() { return {-96.f, 12.f}; }

  static constexpr auto bounding() { return ossia::bounding_mode::FREE; }
};
}
