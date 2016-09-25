#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/detail/algorithms.hpp>

#include <experimental/string_view>
namespace ossia
{
struct address_data
{
  unit_t unit;
  ossia::value value;

  ossia::value_with_unit val;
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

    static_assert(!ossia::is_unit_v<int>, "");
    static_assert(ossia::is_unit_v<ossia::centimeter_u>, "");
    static_assert(ossia::is_unit_v<ossia::rgb_u>, "");
    static_assert(!ossia::is_unit_v<ossia::color_u>, "");
    constexpr ossia::centimeter c{2.3};

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
    QVERIFY(ossia::get_unit_text(col) == "rgb");
    QCOMPARE(ossia::get_unit_text(col), boost::string_ref("rgb"));

    {
      ossia::unit_t some_unit = ossia::distance_u{};
      QVERIFY(parse_unit("cm", some_unit) == ossia::centimeter_u{});
    }

    {
      QVERIFY(parse_unit("cm", ossia::distance_u{}) == ossia::centimeter_u{});
    }

    {
      ossia::unit_t some_unit = ossia::color_u{};
      QVERIFY(parse_unit("cm", some_unit) != ossia::centimeter_u{});
      QVERIFY(parse_unit("cm", some_unit) == ossia::color_u{});
    }
  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
