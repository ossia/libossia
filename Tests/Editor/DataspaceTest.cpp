#include <QtTest>
#include <ossia/ossia.hpp>
#include <ratio>
#include <iostream>
#include <ossia/detail/math.hpp>

using namespace ossia;

namespace ossia
{
// These algorithms are a more statically typed version
// of the ones found in the Jamoma dataspace library.
template<typename Unit>
struct strong_value
{
  using unit_type = Unit;
  using value_type = typename Unit::value_type;
  value_type val;

  constexpr strong_value(value_type f): val{f} { }
  template<typename U,
           typename = std::enable_if_t<
              std::is_same<
               typename U::dataspace_type,
               typename unit_type::dataspace_type>::value>>
  constexpr strong_value(strong_value<U> other):
    val{unit_type::from_neutral(U::to_neutral(other))}
  {
  }

  constexpr strong_value<Unit>(const strong_value<Unit>& other): val{other.val} { }
};

template<typename T, typename Ratio_T>
struct linear_unit : public T
{
  using T::T;
  static constexpr strong_value<typename T::neutral_unit> to_neutral(strong_value<typename T::concrete_type> self)
  {
    return {self.val.value * ratio()};
  }

  static constexpr typename T::value_type from_neutral(strong_value<typename T::neutral_unit> self)
  {
    return self.val.value / ratio();
  }

  static constexpr double ratio() { return double(Ratio_T::num) / double(Ratio_T::den); }
};


template<typename T>
struct meter_ratio;

using meter_u = meter_ratio<std::ratio<1>>;
using kilometer_u = meter_ratio<std::kilo>;
using decimeter_u = meter_ratio<std::deci>;
using centimeter_u = meter_ratio<std::centi>;
using millimeter_u = meter_ratio<std::milli>;
using micrometer_u = meter_ratio<std::micro>;
using nanometer_u = meter_ratio<std::nano>;
using picometer_u = meter_ratio<std::pico>;
using inch_u = meter_ratio<std::ratio<254, 10000>>;
using foot_u = meter_ratio<std::ratio<254 * 12, 10000>>;
using mile_u = meter_ratio<std::ratio<254 * 12 * 5280, 10000>>;

template<typename Impl>
struct distance_unit
{
  using neutral_unit = meter_u;
  using value_type = Float;
  using concrete_type = Impl;
  using dataspace_type = struct distance_dataspace;

protected:
  constexpr distance_unit() = default;
  constexpr distance_unit(const distance_unit&) = default;
  constexpr distance_unit(distance_unit&&) = default;
  constexpr distance_unit& operator=(const distance_unit&) = default;
  constexpr distance_unit& operator=(distance_unit&&) = default;
};

template<typename T>
struct meter_ratio :
    public linear_unit<distance_unit<meter_ratio<T>>, T>
{
  using linear_unit<distance_unit<meter_ratio<T>>, T>::linear_unit;
};

struct cartesian_3d_u;
template<typename Impl>
struct position_unit
{
  using dataspace_type = struct position_dataspace;
  using neutral_unit = cartesian_3d_u;
  using concrete_type = Impl;
};

struct cartesian_3d_u :
    public position_unit<cartesian_3d_u>
{
  using value_type = Vec3f;
  static constexpr strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return self;
  }

  static constexpr value_type from_neutral(strong_value<neutral_unit> self)
  {
    return self.val.value;
  }
};

struct cartesian_2d_u :
    public position_unit<cartesian_2d_u>
{
  using value_type = Vec2f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    return {{{self.val.value[0], self.val.value[1], 0.f}}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    return {{self.val.value[0], self.val.value[1]}};
  }
};

// aed
struct spherical_u :
    public position_unit<spherical_u>
{
  using value_type = Vec3f;
  static strong_value<neutral_unit> to_neutral(strong_value<concrete_type> self)
  {
    const auto deg_to_rad = ossia::pi / 180.;
    auto a = self.val.value[0] * deg_to_rad;
    auto e = self.val.value[1] * deg_to_rad;
    auto d = self.val.value[2];

    auto temp = std::cos(e) * d;

    return {{{
      float(std::sin(a) * temp),
      float(std::cos(a) * temp),
      float(std::sin(e) * d)
    }}};
  }

  static value_type from_neutral(strong_value<neutral_unit> self)
  {
    const auto rad_to_deg = 180. / ossia::pi;
    auto x = self.val.value[0];
    auto y = self.val.value[1];
    auto z = self.val.value[2];

    auto temp = x * x + y * y;

    return {{{
          float(std::atan2(x, y) * rad_to_deg),
          float(std::atan2(z, std::pow(temp, 0.5)) * rad_to_deg),
          float(std::pow(temp + (z * z), 0.5))
        }}};
  }
};

// ad
struct polar_u :
    public position_unit<polar_u>
{
  using value_type = Vec2f;
};

struct opengl_u :
    public position_unit<opengl_u>
{
  using value_type = Vec3f;
};

struct cylindrical_u :
    public position_unit<cylindrical_u>
{
  using value_type = Vec3f;
};

using position_u =
  eggs::variant<cartesian_3d_u, cartesian_2d_u, spherical_u, polar_u, opengl_u, cylindrical_u>;
using cartesian_3d = strong_value<cartesian_3d_u>;
using cartesian_2d = strong_value<cartesian_2d_u>;
using spherical = strong_value<spherical_u>;
using polar = strong_value<polar_u>;
using opengl = strong_value<opengl_u>;
using cylindrical = strong_value<cylindrical_u>;

using position =
  eggs::variant<cartesian_3d, cartesian_2d, spherical, polar, opengl, cylindrical>;

//using strong_value = eggs::variant<distance, position>;
struct address_data
{
  dataspace_unit_t unit;
  ossia::value value;

  //ossia::strong_value val;
};


using meter = strong_value<meter_u>;
using kilometer = strong_value<kilometer_u>;
using decimeter = strong_value<decimeter_u>;
using centimeter = strong_value<centimeter_u>;
using millimeter = strong_value<millimeter_u>;
using micrometer = strong_value<micrometer_u>;
using nanometer = strong_value<nanometer_u>;
using picometer = strong_value<picometer_u>;
using inch = strong_value<inch_u>;
using foot = strong_value<foot_u>;
using mile = strong_value<mile_u>;

using distance_u =
  eggs::variant<
    meter_u, kilometer_u, decimeter_u, centimeter_u, millimeter_u, micrometer_u, nanometer_u, picometer_u, inch_u, foot_u, mile_u>;

using distance =
eggs::variant<
  meter, kilometer, decimeter, centimeter, millimeter, micrometer, nanometer, picometer, inch, foot, mile>;

}


class DataspaceTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  /*! test impulse */
  void test_dataspace()
  {
    // Dataspace : enforces a family of units
    // Unit : enforces a certain type of storage

    constexpr ossia::centimeter c{{2.3}};

    constexpr ossia::millimeter m = c;

    static_assert(m.val.value == 23., "");

    static_assert(qFuzzyCompare(ossia::centimeter{ossia::inch{1}}.val.value, 2.54f), "");
    static_assert(qFuzzyCompare(ossia::meter{ossia::mile{1}}.val.value, 1609.34f), "");


    // Ex. 1 : making an automation of the correct type ? e.g. circular for circular units...
    // have some kind of unit transformer ?
    // an automation on circular units should yield a circle.
    // functionnally : autom value -> unit transformer -> address
    // we are interested in :
    // - units as metadata to show up the correct widgets, etc
    // - unit transformation functions to do meaningful automations
    // - units as a c++ type to write stuff using the ossia api.

    // the unit stored in the address is the unit in which what we currently have must be converted into.
    // what should happen if there is a type error ?
    // if we push a simple "ossia::value" and its underlying type matches the unit's value_type,
    // we assume it is of the correct unit
    // if we push a value of the same dataspace, we convert it

    // Maybe the "unit" types shouldn't hold a value, but just have a value_type member ?
  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
