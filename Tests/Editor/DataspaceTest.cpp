#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/detail/dataspace_convert.hpp>
#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <ossia/editor/dataspace/detail/dataspace_parse.hpp>
#include <ossia/detail/algorithms.hpp>
#include <brigand/algorithms/for_each.hpp>

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

  template<typename T>
  void test_conversions_impl()
  {
    // Conversion
    using dataspace_type = ossia::distance;
    brigand::for_each<dataspace_type>([&] (auto unit_1)
    {
      using unit_1_type = typename decltype(unit_1)::type;
      unit_1_type unit_1_v;

      brigand::for_each<dataspace_type>([&] (auto unit_2)
      {
        using unit_2_type = typename decltype(unit_2)::type;

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
  }
private Q_SLOTS:

  /*! test impulse */
  void test_dataspace()
  {
#if !defined(_MSC_VER)
    // Dataspace : enforces a family of units
    // Unit : enforces a certain type of storage
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

#endif
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

  void test_conversions_at_construction()
  {
    ossia::unit_t unit;

    // Construction
    brigand::for_each<ossia::unit_t>([&] (auto t) {
      brigand::for_each<typename decltype(t)::type>([&] (auto u) {
        unit = typename decltype(u)::type{};
      });
    });
  }

  void test_conversions()
  {
    test_conversions_impl<ossia::distance>();
    test_conversions_impl<ossia::angle>();
    test_conversions_impl<ossia::color>();
    test_conversions_impl<ossia::position>();
    test_conversions_impl<ossia::orientation>();
    test_conversions_impl<ossia::speed>();
    test_conversions_impl<ossia::gain>();
    test_conversions_impl<ossia::time>();
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

  void test_get_dataspace_text()
  {
    using namespace ossia;

    {
      QVERIFY(get_dataspace_text({}).empty());

      QVERIFY(get_dataspace_text(ossia::color_u{}) == "color");
      QVERIFY(get_dataspace_text(ossia::rgb_u{}) == "color");

      QVERIFY(get_dataspace_text(ossia::centimeter_u{}) == "distance");
      QVERIFY(get_dataspace_text(ossia::distance_u{}) == "distance");
    }

    {
      QVERIFY(get_unit_text({}).empty());

      QVERIFY(get_unit_text(ossia::color_u{}).empty());
      QVERIFY(get_unit_text(ossia::rgb_u{}) == "rgb");

      QVERIFY(get_unit_text(ossia::distance_u{}).empty());
      QVERIFY(get_unit_text(ossia::centimeter_u{}) == "cm");
    }

    {
      QVERIFY(get_pretty_unit_text({}) == "none");

      QVERIFY(get_pretty_unit_text(ossia::color_u{}) == "color");
      QVERIFY(get_pretty_unit_text(ossia::rgb_u{}) == "color.rgb");

      QVERIFY(get_pretty_unit_text(ossia::distance_u{}) == "distance");
      QVERIFY(get_pretty_unit_text(ossia::centimeter_u{}) == "distance.cm");
    }

    {
      QVERIFY(get_unit_accessor({}, 0) == 0);
      QVERIFY(get_unit_accessor({}, -1) == 0);
      QVERIFY(get_unit_accessor({}, 1) == 0);
      QVERIFY(get_unit_accessor({}, 255) == 0);

      QVERIFY(get_unit_accessor(ossia::color_u{}, 0) == 0);
      QVERIFY(get_unit_accessor(ossia::distance_u{}, 0) == 0);

      QVERIFY(get_unit_accessor(ossia::centimeter_u{}, 0) == 0);
      QVERIFY(get_unit_accessor(ossia::centimeter_u{}, -1) == 0);
      QVERIFY(get_unit_accessor(ossia::centimeter_u{}, 1) == 0);
      QVERIFY(get_unit_accessor(ossia::centimeter_u{}, 255) == 0);

      QVERIFY(get_unit_accessor(ossia::rgb_u{}, 0) == 'r');
      QVERIFY(get_unit_accessor(ossia::rgb_u{}, 1) == 'g');
      QVERIFY(get_unit_accessor(ossia::rgb_u{}, 2) == 'b');
      QVERIFY(get_unit_accessor(ossia::rgb_u{}, 3) == 0);
      QVERIFY(get_unit_accessor(ossia::rgb_u{}, -1) == 0);
      QVERIFY(get_unit_accessor(ossia::rgb_u{}, 255) == 0);

    }

    {
      QVERIFY(parse_dataspace("") == ossia::unit_t{});
      QVERIFY(parse_dataspace("none") == ossia::unit_t{});
      QVERIFY(parse_dataspace("color") == ossia::color_u{});
      QVERIFY(parse_dataspace("distance") == ossia::distance_u{});
    }

    {
      QVERIFY(parse_unit("rgb", ossia::unit_t{}) == ossia::unit_t{});
      QVERIFY(parse_unit("rgb", ossia::color_u{}) == ossia::rgb_u{});
      QVERIFY(parse_unit("rgb", ossia::distance_u{}) == ossia::distance_u{});
      QVERIFY(parse_unit("rgb", ossia::unit_t{ossia::color_u{}}) == ossia::rgb_u{});
      QVERIFY(parse_unit("rgb", ossia::unit_t{ossia::distance_u{}}) == ossia::distance_u{});
      // should not compile : QVERIFY(parse_unit("rgb", ossia::rgb_u{}) == ossia::rgb_u{});
      // should not compile : QVERIFY(parse_unit("rgb", ossia::hsv_u{}) == ossia::rgb_u{});
    }

    {
      QVERIFY(parse_pretty_unit("") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("none") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("rgb") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("color.rgb") == ossia::rgb_u{});
      QVERIFY(parse_pretty_unit("color.tata") == ossia::color_u{});
      QVERIFY(parse_pretty_unit("color.cm") == ossia::color_u{});
      QVERIFY(parse_pretty_unit("distance.cm") == ossia::centimeter_u{});
    }

    {
      QVERIFY(make_value(ossia::Int{10}, ossia::centimeter_u{}) == ossia::centimeter{10});
      QVERIFY(make_value(ossia::Char{10}, ossia::centimeter_u{}) == ossia::centimeter{10});
      QVERIFY(make_value(ossia::Bool{true}, ossia::centimeter_u{}) == ossia::centimeter{1});
      QVERIFY(make_value(ossia::Float{1.2}, ossia::centimeter_u{}) == ossia::centimeter{1.2});
      QVERIFY(make_value(ossia::Float{1.2}, ossia::rgb_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(ossia::Impulse{}, ossia::rgb_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(ossia::Impulse{}, ossia::rgba_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(ossia::Impulse{}, ossia::cartesian_2d_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(make_vec(1.2, 1.3, 32.5), ossia::rgb_u{}) == ossia::rgb{make_vec(1.2, 1.3, 32.5)});
      QVERIFY(make_value(make_vec(1.2, 1.3, 32.5, 0.7), ossia::rgba_u{}) == ossia::rgba{make_vec(1.2, 1.3, 32.5, 0.7)});
      QVERIFY(make_value(make_vec(1.2, 1.3), ossia::cartesian_2d_u{}) == ossia::cartesian_2d{make_vec(1.2, 1.3)});
      QVERIFY(make_value(ossia::Tuple{Float{1.2}, Float{1.3}, Float{32.5}}, ossia::rgb_u{}) == ossia::rgb{make_vec(1.2, 1.3, 32.5)});
    }

    {
      QVERIFY(make_unit(0, 0) == ossia::meter_u{});
      QVERIFY(make_unit(1, 0) == ossia::cartesian_3d_u{});
      QVERIFY(make_unit(1, 1) == ossia::cartesian_2d_u{});
      QVERIFY(make_unit(-1, -1) == ossia::unit_t{});
      QVERIFY(make_unit(-1, 0) == ossia::unit_t{});
      QVERIFY(make_unit(0, -1) == ossia::unit_t{});
      QVERIFY(make_unit(0, 100) == ossia::unit_t{});
      QVERIFY(make_unit(100, 0) == ossia::unit_t{});
      QVERIFY(make_unit(100, 100) == ossia::unit_t{});
    }

    {
      QVERIFY(matching_type(ossia::unit_t{}) == ossia::val_type::IMPULSE);
      QVERIFY(matching_type(ossia::distance_u{}) == ossia::val_type::IMPULSE);
      QVERIFY(matching_type(ossia::color_u{}) == ossia::val_type::IMPULSE);
      QVERIFY(matching_type(ossia::rgb_u{}) == ossia::val_type::VEC3F);
      QVERIFY(matching_type(ossia::rgba_u{}) == ossia::val_type::VEC4F);
      QVERIFY(matching_type(ossia::centimeter_u{}) == ossia::val_type::FLOAT);
    }

    {
      QVERIFY(convert(ossia::rgb{make_vec(1.2, 1.3, 32.5)}, ossia::bgr_u{}) == ossia::bgr{make_vec(32.5, 1.3, 1.2)});
      QVERIFY(convert(ossia::rgb{make_vec(1.2, 1.3, 32.5)}, ossia::centimeter_u{}) == ossia::value_with_unit{});
      QVERIFY(convert(ossia::value_with_unit{}, ossia::centimeter_u{}) == ossia::value_with_unit{});
    }

    {
      QVERIFY(to_value(ossia::value_with_unit{}) == ossia::value{});
      QVERIFY(to_value(ossia::rgb{make_vec(1.2, 1.3, 32.5)}) == make_vec(1.2, 1.3, 32.5));
      QVERIFY(to_value(ossia::centimeter{2.3}) == Float{2.3});
    }

    {
      QVERIFY(to_unit(ossia::value_with_unit{}) == ossia::unit_t{});
      QVERIFY(to_unit(ossia::rgb{make_vec(1.2, 1.3, 32.5)}) == ossia::rgb_u{});
      QVERIFY(to_unit(ossia::centimeter{2.3}) == ossia::centimeter_u{});
    }

    get_unit_parser();
  }

  void convert_benchmark()
  {
    const int N = 100000;
    auto t1 = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < N; i++)
      ossia::convert(ossia::rgb{ossia::make_vec(1.2, 1.3, 32.5)}, ossia::hsv_u{});
    auto t2 = std::chrono::high_resolution_clock::now();

    qDebug() << "convert time: "
             << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / double(N);
  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
