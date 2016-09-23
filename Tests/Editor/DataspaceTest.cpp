#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{

struct address_data
{
  unit_t unit;
  ossia::value value;

  ossia::value_with_unit val;
};

}

struct foo
{
  static const constexpr auto vals{ossia::make_array("foo", "bar")};
};

struct unit_text_visitor
{
  template<typename... Args>
  boost::string_ref operator()(const eggs::variant<Args...>& dataspace)
  {
    if(dataspace)
      return eggs::variants::apply(*this, dataspace);
    else
      return "";
  }

  template<typename Unit>
  boost::string_ref operator()(Unit)
  {
    return ossia::unit_traits<Unit>::text()[0];
  }
  /*
  // Angle
  boost::string_ref operator()(ossia::radian_u) { return "rad"; }
  boost::string_ref operator()(ossia::degree_u) { return "deg"; }

  // Color
  boost::string_ref operator()(ossia::argb_u) { return "argb"; }
  boost::string_ref operator()(ossia::rgba_u) { return "rgba"; }
  boost::string_ref operator()(ossia::rgb_u) { return "rgb"; }
  boost::string_ref operator()(ossia::bgr_u) { return "bgr"; }
  boost::string_ref operator()(ossia::argb8_u) { return "argb8"; }
  boost::string_ref operator()(ossia::hsv_u) { return "hsv"; }
  boost::string_ref operator()(ossia::hsl_u) { return "hsl"; }
  boost::string_ref operator()(ossia::cmy8_u) { return "cmy8"; }
  boost::string_ref operator()(ossia::cmyk8_u) { return "cmyk8"; }
  boost::string_ref operator()(ossia::xyz_u) { return "xyz"; }
  boost::string_ref operator()(ossia::yxy_u) { return "yxy"; }
  boost::string_ref operator()(ossia::hunter_lab_u) { return "hunter_lab"; }
  boost::string_ref operator()(ossia::cie_lab_u) { return "cie_lab"; }
  boost::string_ref operator()(ossia::cie_luv_u) { return "cie_luv"; }

  // Distance
  boost::string_ref operator()(ossia::meter_u) { return "m"; }
  boost::string_ref operator()(ossia::kilometer_u) { return "km"; }
  boost::string_ref operator()(ossia::decimeter_u) { return "dm"; }
  boost::string_ref operator()(ossia::centimeter_u) { return "cm"; }
  boost::string_ref operator()(ossia::millimeter_u) { return "mm"; }
  boost::string_ref operator()(ossia::micrometer_u) { return "um"; }
  boost::string_ref operator()(ossia::nanometer_u) { return "nm"; }
  boost::string_ref operator()(ossia::picometer_u) { return "pm"; }
  boost::string_ref operator()(ossia::inch_u) { return "inch"; }
  boost::string_ref operator()(ossia::foot_u) { return "foot"; }
  boost::string_ref operator()(ossia::mile_u) { return "mile"; }

  // Orientation
  boost::string_ref operator()(ossia::quaternion_u) { return "quaternion"; }
  boost::string_ref operator()(ossia::euler_u) { return "euler"; }
  boost::string_ref operator()(ossia::axis_u) { return "axis"; }

  // Position
  boost::string_ref operator()(ossia::cartesian_3d_u) { return "xyz"; }
  boost::string_ref operator()(ossia::cartesian_2d_u) { return "xy"; }
  boost::string_ref operator()(ossia::spherical_u) { return "spherical"; }
  boost::string_ref operator()(ossia::polar_u) { return "polar"; }
  boost::string_ref operator()(ossia::opengl_u) { return "openGL"; }
  boost::string_ref operator()(ossia::cylindrical_u) { return "cylindrical"; }

  // Speed
  boost::string_ref operator()(ossia::meter_per_second_u) { return "m/s"; }
  boost::string_ref operator()(ossia::miles_per_hour_u) { return "mph"; }
  boost::string_ref operator()(ossia::kilometer_per_hour_u) { return "km/h"; }
  boost::string_ref operator()(ossia::knot_u) { return "kn"; }
  boost::string_ref operator()(ossia::foot_per_second_u) { return "ft/s"; }
  boost::string_ref operator()(ossia::foot_per_hour_u) { return "ft/h"; }
  */
};


struct unit_factory_visitor
{
  boost::string_ref text;
/*
  ossia::unit_t operator()(ossia::color_u)
  {
    static const std::unordered_map<std::string, ossia::unit_t> units
    {
      {
    }
  }*/
};

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


    static_assert(qFuzzyCompare(ossia::kilometer_per_hour{ossia::meter_per_second{1}}.val.value, 3.6f), "");
    static_assert(qFuzzyCompare(ossia::miles_per_hour{ossia::meter_per_second{1}}.val.value, 2.236936f), "");
    static_assert(qFuzzyCompare(ossia::foot_per_second{ossia::meter_per_second{1}}.val.value, 3.280840f), "");
    static_assert(qFuzzyCompare(ossia::knot{ossia::meter_per_second{1}}.val.value, 1.943844f), "");

    static_assert(qFuzzyCompare(ossia::radian{ossia::degree{180}}.val.value, 3.14159f), "");
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

    ossia::unit_t u;
    u = ossia::color_u{};
    u = ossia::rgb_u{};
    u = ossia::decimeter_u{};

    ossia::color_u col;
    col = ossia::rgb_u{};
    // must not compile : col = ossia::decimeter_u{};

    // With C++17 : static_assert(eggs::variants::apply(unit_text_visitor{}, col) == boost::string_ref("rgb"), "");
    QCOMPARE(eggs::variants::apply(unit_text_visitor{}, col), boost::string_ref("rgb"));
  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
