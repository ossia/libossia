// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/detail/dataspace_convert.hpp>
#include <ossia/network/dataspace/detail/dataspace_merge.hpp>
#include <ossia/network/dataspace/detail/dataspace_parse.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/for_each.hpp>

namespace ossia
{
struct parameter_data
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
    ossia::for_each_tagged(T{}, [&] (auto unit_1)
    {
      using unit_1_type = typename decltype(unit_1)::type;
      unit_1_type unit_1_v;

      ossia::for_each_tagged(T{}, [&] (auto unit_2)
      {
        using unit_2_type = typename decltype(unit_2)::type;
        // Conversion at construction
        unit_2_type unit_2_v(unit_1_v);
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
  void test_dataspace_simple()
  {
    static_assert(std::is_same<
      ossia::centimeter::dataspace_type,
      ossia::millimeter::dataspace_type>::value, "invalid");
    //using t1 = ossia::enable_if_same_dataspace<ossia::centimeter, ossia::millimeter>;
    //using t2 = ossia::enable_if_same_dataspace<ossia::rgb, ossia::millimeter>;

    ossia::centimeter c{2.3};

    ossia::millimeter m = c;
    QCOMPARE(m.dataspace_value, 23.f);
  }

  void test_dataspace()
  {
#if !defined(_MSC_VER)
    // Dataspace : enforces a family of units
    // Unit : enforces a certain type of storage
    constexpr ossia::centimeter c{2.3};

    constexpr ossia::millimeter m = c;

    static_assert(m.dataspace_value == 23., "");

    static_assert(qFuzzyCompare(ossia::centimeter{ossia::inch{1.}}.dataspace_value, 2.54f), "");
    static_assert(qFuzzyCompare(ossia::meter{ossia::mile{1.}}.dataspace_value, 1609.34f), "");


    static_assert(qFuzzyCompare(ossia::kilometer_per_hour{ossia::meter_per_second{1.}}.dataspace_value, 3.6f), "");
    static_assert(qFuzzyCompare(ossia::miles_per_hour{ossia::meter_per_second{1.}}.dataspace_value, 2.236936f), "");
    static_assert(qFuzzyCompare(ossia::foot_per_second{ossia::meter_per_second{1.}}.dataspace_value, 3.280840f), "");
    static_assert(qFuzzyCompare(ossia::knot{ossia::meter_per_second{1}}.dataspace_value, 1.943844f), "");

    static_assert(qFuzzyCompare(ossia::radian{ossia::degree{180}}.dataspace_value, 3.14159f), "");

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
    ossia::unit_t x;
    x = col;
    QVERIFY(x.which() == x.v.Type5);
    QVERIFY(x.v.m_impl.m_value5.m_type == ossia::color_u::Type::Type2);
    // must not compile : col = ossia::decimeter_u{};

    // With C++17 : static_assert(eggs::variants::apply(unit_text_visitor{}, col) == ossia::string_view("rgb"), "");
    QVERIFY(ossia::get_unit_text(col) == "rgb");
    QCOMPARE(ossia::get_unit_text(col), ossia::string_view("rgb"));

    {
      ossia::unit_t some_unit = ossia::distance_u{};
      QVERIFY(parse_unit("cm", some_unit) == ossia::centimeter_u{});
    }

    {
      QVERIFY(parse_unit("cm", ossia::distance_u{}) == ossia::centimeter_u{});
    }

    {
      QVERIFY(ossia::unit_t{ossia::color_u{}} != ossia::unit_t{ossia::centimeter_u{}});
      QVERIFY(ossia::unit_t{ossia::color_u{}} != ossia::centimeter_u{});

      ossia::unit_t some_unit = ossia::color_u{};
      auto parsed_unit = parse_unit("cm", some_unit);
      QVERIFY(parsed_unit != ossia::centimeter_u{});
      QVERIFY(parsed_unit != ossia::color_u{});
    }

#endif
  }

  void test_conversions_at_construction()
  {
    ossia::unit_t unit;

    // Construction
    ossia::for_each_tagged(ossia::dataspace_u_list{}, [&] (auto t) {
      ossia::for_each_tagged(typename decltype(t)::type{}, [&] (auto u) {
        unit = typename decltype(u)::type{};
      });
    });
  }

  void test_conversions()
  {
    test_conversions_impl<ossia::distance_list>();
    test_conversions_impl<ossia::angle_list>();
    test_conversions_impl<ossia::color_list>();
    test_conversions_impl<ossia::position_list>();
    test_conversions_impl<ossia::orientation_list>();
    test_conversions_impl<ossia::speed_list>();
    test_conversions_impl<ossia::gain_list>();
    test_conversions_impl<ossia::time_list>();
  }

  void test_visitors()
  {
    // get_unit_text
    ossia::get_unit_text(ossia::unit_t{});
    ossia::for_each_tagged(ossia::dataspace_u_list{}, [&] (auto t) {
      using dataspace_t = typename ossia::matching_unit_u_list<typename decltype(t)::type>::type;
      ossia::get_unit_text(dataspace_t{});
      ossia::for_each_tagged(typename decltype(t)::type{}, [&] (auto u) {
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

    ossia::for_each_tagged(ossia::dataspace_u_list{}, [&] (auto t) {
      using dataspace_type = typename decltype(t)::type;
      ossia::for_each_tagged(dataspace_type{}, [&] (auto u) {
        using unit_type = typename decltype(u)::type;
        auto unit_text_array = ossia::unit_traits<unit_type>::text();
        for(auto unit_text : unit_text_array)
        {
          using dataspace_correct_type = typename ossia::matching_unit_u_list<dataspace_type>::type;
          auto parsed_unit = ossia::parse_unit(unit_text, dataspace_correct_type{});
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
      QVERIFY(parse_unit("rgb", ossia::distance_u{}) == ossia::unit_t{});
      QVERIFY(parse_unit("rgb", ossia::unit_t{ossia::color_u{}}) == ossia::rgb_u{});
      QVERIFY(parse_unit("rgb", ossia::unit_t{ossia::distance_u{}}) == ossia::unit_t{});
      // should not compile : QVERIFY(parse_unit("rgb", ossia::rgb_u{}) == ossia::rgb_u{});
      // should not compile : QVERIFY(parse_unit("rgb", ossia::hsv_u{}) == ossia::rgb_u{});
    }

    {
      QVERIFY(parse_pretty_unit("") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("plouf") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("none") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("rgb") == ossia::rgb_u{});
      QVERIFY(parse_pretty_unit("color.rgb") == ossia::rgb_u{});
      QVERIFY(parse_pretty_unit("color.tata") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("color.cm") == ossia::unit_t{});
      QVERIFY(parse_pretty_unit("distance.cm") == ossia::centimeter_u{});

      QVERIFY(!ossia::unit_t{});
      QVERIFY(!parse_pretty_unit("plouf"));
      QVERIFY(!parse_pretty_unit("time.plouf"));
    }

    {
qDebug() << ossia::to_pretty_string(make_value(int32_t{ 10 }, ossia::centimeter_u{})).c_str();
      QVERIFY(make_value(int32_t{10}, ossia::centimeter_u{}) == ossia::value_with_unit{ossia::centimeter{10}});
      QVERIFY(make_value(char{10}, ossia::centimeter_u{}) == ossia::value_with_unit{ossia::centimeter{10}});
      QVERIFY(make_value(bool{true}, ossia::centimeter_u{}) == ossia::value_with_unit{ossia::centimeter{1}});
      QVERIFY(make_value(1.2, ossia::centimeter_u{}) == ossia::value_with_unit{ossia::centimeter{1.2}});
      QVERIFY(make_value(1.2, ossia::rgb_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(ossia::impulse{}, ossia::rgb_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(ossia::impulse{}, ossia::rgba_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(ossia::impulse{}, ossia::cartesian_2d_u{}) == ossia::value_with_unit{});
      QVERIFY(make_value(make_vec(1.2, 1.3, 32.5), ossia::rgb_u{}) == ossia::value_with_unit{ossia::rgb{make_vec(1.2, 1.3, 32.5)}});
      QVERIFY(make_value(make_vec(1.2, 1.3, 32.5, 0.7), ossia::rgba_u{}) == ossia::value_with_unit{ossia::rgba{make_vec(1.2, 1.3, 32.5, 0.7)}});
      QVERIFY(make_value(make_vec(1.2, 1.3), ossia::cartesian_2d_u{}) == ossia::value_with_unit{ossia::cartesian_2d{make_vec(1.2, 1.3)}});
      QVERIFY(make_value(std::vector<ossia::value>{1.2, 1.3, 32.5}, ossia::rgb_u{}) == ossia::value_with_unit{ossia::rgb{make_vec(1.2, 1.3, 32.5)}});

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
      QVERIFY(convert(ossia::rgb{make_vec(1.2, 1.3, 32.5)}, ossia::bgr_u{}) == ossia::value_with_unit{ossia::bgr{make_vec(32.5, 1.3, 1.2)}});
      QVERIFY(convert(ossia::rgb{make_vec(1.2, 1.3, 32.5)}, ossia::centimeter_u{}) == ossia::value_with_unit{});
      QVERIFY(convert(ossia::value_with_unit{}, ossia::centimeter_u{}) == ossia::value_with_unit{});
    }

    {
      QVERIFY(to_value(ossia::value_with_unit{}) == ossia::value{});
      QVERIFY(to_value(ossia::rgb{make_vec(1.2, 1.3, 32.5)}) == make_vec(1.2, 1.3, 32.5));
      QVERIFY(to_value(ossia::centimeter{2.3}) == 2.3);
    }

    {
      QVERIFY(to_unit(ossia::value_with_unit{}) == ossia::unit_t{});
      QVERIFY(to_unit(ossia::rgb{make_vec(1.2, 1.3, 32.5)}) == ossia::rgb_u{});
      QVERIFY(to_unit(ossia::centimeter{2.3}) == ossia::centimeter_u{});
    }

    get_unit_parser();
  }

  void test_convertible()
  {
    using namespace ossia;
    QVERIFY(check_units_convertible(ossia::rgb_u{}, ossia::hsv_u{}));
    QVERIFY(check_units_convertible(ossia::rgb_u{}, ossia::rgb_u{}));
    QVERIFY(!check_units_convertible(ossia::rgb_u{}, ossia::degree_u{}));
    QVERIFY(!check_units_convertible(ossia::rgb_u{}, ossia::cartesian_3d_u{}));
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
