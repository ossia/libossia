#include <QtTest>
#include <ossia/ossia.hpp>
#include <ratio>
#include <iostream>

using namespace ossia;

namespace ossia
{
template<typename T, typename Ratio_T>
struct linear_unit : public T
{
  using T::T;
  static constexpr typename T::neutral_type to_neutral(typename T::concrete_type self)
  {
    return {self.val.value * ratio()};
  }

  static constexpr typename T::value_type from_neutral(typename T::neutral_type self)
  {
    return self.val.value / ratio();
  }

  static constexpr double ratio() { return double(Ratio_T::num) / double(Ratio_T::den); }
};


template<typename T>
struct meter_ratio;

using meter = meter_ratio<std::ratio<1>>;
using kilometer = meter_ratio<std::kilo>;
using decimeter = meter_ratio<std::deci>;
using centimeter = meter_ratio<std::centi>;
using millimeter = meter_ratio<std::milli>;
using micrometer = meter_ratio<std::micro>;
using nanometer = meter_ratio<std::nano>;
using picometer = meter_ratio<std::pico>;
using inch = meter_ratio<std::ratio<254, 10000>>;
using foot = meter_ratio<std::ratio<254 * 12, 10000>>;
using mile = meter_ratio<std::ratio<254 * 12 * 5280, 10000>>;

template<typename Impl>
struct distance_unit
{
  using neutral_type = meter;
  using value_type = Float;
  using concrete_type = Impl;

  Float val; // ok for distance, not for color

  constexpr distance_unit(Float f): val{f} { }
  template<typename U, typename = std::enable_if_t<std::is_same<decltype(U::to_neutral({})), neutral_type>::value>>
  constexpr distance_unit(U other): val{Impl::from_neutral(U::to_neutral(other))} { }

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

using distance =
  eggs::variant<meter, kilometer, decimeter, centimeter, millimeter, micrometer, nanometer, picometer, inch, foot, mile>;

struct cartesian_3d;
struct position_unit
{
  using neutral_type = cartesian_3d;
};

struct cartesian_3d :
    public position_unit
{
  Vec3f val;
};

struct cartesian_2d :
    public position_unit
{
  Vec2f val;
};

// aed
struct spherical :
    public position_unit
{
  Vec3f val;
};

// ad
struct polar :
    public position_unit
{
  Vec2f val;
};

struct opengl :
    public position_unit
{
  Vec3f val;
};

struct cylindrical :
    public position_unit
{
  Vec3f val;
};

using position =
  eggs::variant<cartesian_3d, cartesian_2d, spherical, polar, opengl, cylindrical>;

using strong_value = eggs::variant<distance, position>;
struct address_data
{
  dataspace_unit_t unit;
  ossia::value value;

  //ossia::strong_value val;
};

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

  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
