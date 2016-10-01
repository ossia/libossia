#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/detail/dataspace_convert.hpp>
#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <ossia/editor/dataspace/detail/dataspace_parse.hpp>
#include <ossia/detail/algorithms.hpp>
#include <brigand/algorithms/for_each.hpp>
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
    static_assert(ossia::detail::is_iterable_v<decltype(ossia::Vec3f::value)>, "");
    static_assert(!ossia::detail::is_iterable_v<decltype(ossia::Float::value)>, "");
    static_assert(!ossia::detail::is_iterable_v<const decltype(ossia::Float::value)>, "");

    static_assert(!ossia::is_unit_v<int>, "");
    static_assert(ossia::is_unit_v<ossia::centimeter_u>, "");
    static_assert(ossia::is_unit_v<ossia::rgb_u>, "");
    static_assert(!ossia::is_unit_v<ossia::color_u>, "");
    constexpr ossia::centimeter c{2.3};

    constexpr ossia::millimeter m = c;

    static_assert(m.value.value == 23., "");

    static_assert(qFuzzyCompare(ossia::centimeter{ossia::inch{1}}.value.value, 2.54f), "");
    static_assert(qFuzzyCompare(ossia::meter{ossia::mile{1}}.value.value, 1609.34f), "");


    static_assert(qFuzzyCompare(ossia::kilometer_per_hour{ossia::meter_per_second{1}}.value.value, 3.6f), "");
    static_assert(qFuzzyCompare(ossia::miles_per_hour{ossia::meter_per_second{1}}.value.value, 2.236936f), "");
    static_assert(qFuzzyCompare(ossia::foot_per_second{ossia::meter_per_second{1}}.value.value, 3.280840f), "");
    static_assert(qFuzzyCompare(ossia::knot{ossia::meter_per_second{1}}.value.value, 1.943844f), "");

    static_assert(qFuzzyCompare(ossia::radian{ossia::degree{180}}.value.value, 3.14159f), "");
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

    // With C++17 : static_assert(eggs::variants::apply(unit_text_visitor{}, col) == boost::string_view("rgb"), "");
    QVERIFY(ossia::get_unit_text(col) == "rgb");
    QCOMPARE(ossia::get_unit_text(col), boost::string_view("rgb"));

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

  void test_conversions()
  {
    ossia::unit_t unit;

    // Construction
    brigand::for_each<ossia::unit_t>([&] (auto t) {
      brigand::for_each<typename decltype(t)::type>([&] (auto u) {
        unit = typename decltype(u)::type{};
      });
    });

    // Conversion
    /* TODO
    brigand::for_each<ossia::value_with_unit>([&] (auto t)
    {
      using dataspace_type = typename decltype(t)::type;

      brigand::for_each<dataspace_type>([&] (auto unit_1)
      {
        using unit_1_type = typename decltype(unit_1)::type;
        unit_1_type unit_1_v;

        brigand::for_each<dataspace_type>([&] (auto unit_2)
        {
          using unit_2_type = typename decltype(unit_1)::type;

          // Conversion at construction
          unit_2_type unit_2_v = unit_1_v;
          (void)unit_2_v;

          // Conversion by convert function
          auto res = convert(unit_1_v, typename unit_2_type::unit_type{});
          auto val = to_value(res);
          (void) val;
          auto str = to_pretty_string(res);
          (void) str;
        });
      });
    });
    */
  }

  void test_visitors()
  {
    // get_unit_text
    ossia::get_unit_text(ossia::unit_t{});
    brigand::for_each<ossia::unit_t>([&] (auto t) {
      ossia::get_unit_text(typename decltype(t)::type{});
      brigand::for_each<typename decltype(t)::type>([&] (auto u) {
        ossia::get_unit_text(typename decltype(u)::type{});
      });
    });

    // parse_dataspace
    QVERIFY(ossia::parse_dataspace("color") == ossia::color_u{});
    QVERIFY(ossia::parse_dataspace("tata") == ossia::unit_t{});
    QVERIFY(ossia::parse_dataspace("") == ossia::unit_t{});

    // parse_unit
    auto p1 = ossia::parse_unit("tutu", ossia::unit_t{});
    QVERIFY(!p1);
    auto p2 = ossia::parse_unit("rgb", ossia::unit_t{});
    QVERIFY(!p2);

    brigand::for_each<ossia::unit_t>([&] (auto t) {
      using dataspace_type = typename decltype(t)::type;
      brigand::for_each<dataspace_type>([&] (auto u) {
        using unit_type = typename decltype(u)::type;
        auto unit_text_array = ossia::unit_traits<unit_type>::text();
        for(auto unit_text : unit_text_array)
        {
          auto parsed_unit = ossia::parse_unit(unit_text, dataspace_type{});
          QVERIFY(parsed_unit == unit_type{});
        }
      });
    });

  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
