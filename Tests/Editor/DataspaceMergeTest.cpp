#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <QtTest>
#include "TestUtils.hpp"
class DataspaceMergeTest : public QObject
{
  Q_OBJECT

  void debug(const ossia::value& t) { qDebug() << ossia::value_to_pretty_string(t).c_str(); }

  template<int N>
  bool fuzzy_compare(ossia::Vec<float, N> v1, ossia::Vec<float, N> v2)
  {
    bool b = true;
    double eps = 1e-6;
    for(int i = 0; i < N; i++)
    {
      if(std::abs(v1.value[i] - v2.value[i]) > eps)
        b = false;
    }
    return b;
  }

  template<typename T>
  void make_bitset_impl(T& bs, int pos)
  {
  }
  template<typename T, typename Arg, typename... Args>
  void make_bitset_impl(T& bs, int pos,  Arg&& arg, Args&&... args)
  {
    if(arg)
      bs.set(pos);

    make_bitset_impl(bs, pos+1, args...);
  }

  template<typename... Args>
  auto make_bitset(Args... args)
    -> std::bitset<sizeof...(Args)>
  {
    std::bitset<sizeof...(Args)> bs;
    make_bitset_impl(bs, 0, args...);
    return bs;
  }

private Q_SLOTS:
  void static_test()
  {
    static_assert(ossia::detail::is_iterable_v<decltype(ossia::Vec3f::value)>, "");
    static_assert(!ossia::detail::is_iterable_v<decltype(ossia::Float::value)>, "");
    static_assert(!ossia::detail::is_iterable_v<const decltype(ossia::Float::value)>, "");

    static_assert(!ossia::detail::is_iterable_v<decltype(ossia::centimeter::value)>, "");
    static_assert(ossia::detail::is_iterable_v<decltype(ossia::rgb::value)>, "RGB is not iterable");

    static_assert(!ossia::is_unit_v<int>, "");
    static_assert(ossia::is_unit_v<ossia::centimeter_u>, "");
    static_assert(ossia::is_unit_v<ossia::rgb_u>, "");
    static_assert(!ossia::is_unit_v<ossia::color_u>, "");
  }

  void convert_test()
  {
    ossia::TestUtils t;

    // Cases :
    /////// Address float, no unit ///////
    t.float_addr->pushValue(ossia::Float(0.));

    // Message float, no unit
    {
      ossia::Float expected(3.2);
      ossia::message m{*t.float_addr, expected, {}};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }

    // Message float, unit
    {
      ossia::Float expected(3.2);
      ossia::message m{*t.float_addr, expected, ossia::meter_per_second_u{}};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }


    /////// Address float, unit ///////
    t.float_addr->setUnit(ossia::meter_per_second_u{});

    // Message float, no unit
    {
      ossia::Float expected(3.2);
      ossia::message m{*t.float_addr, expected, {}};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }

    // Message float, same unit
    {
      ossia::Float expected(3.2);
      ossia::message m{*t.float_addr, expected, ossia::meter_per_second_u{}};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }

    // Message float, different unit and same dataspace
    {
      ossia::message m{*t.float_addr, ossia::Float(1.), ossia::kilometer_per_hour_u{}};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == ossia::Float(1. / 3.6)); // 1km/h -> 0.27777 m/s
    }

    // Message float, different dataspace
    {
      // The message is ignored, we keep the previous value
      ossia::message m{*t.float_addr, ossia::Float(1234.), ossia::meter_u{}};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == ossia::Float(1. / 3.6));
    }


    /////// Address vec, no unit ///////
    t.vec3f_addr->pushValue(ossia::make_vec(0., 0., 0.));

    // Message float, no index, no unit
    {
      // The message is ignored, we keep the previous value
      ossia::message m{*t.vec3f_addr, ossia::Float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 0., 0.));
    }

    // Message float, no index, unit
    {
      // The message is ignored, we keep the previous value
      ossia::message m{*t.vec3f_addr, ossia::Float(1234.), ossia::rgb_u{}}; // shouldn't be possible
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 0., 0.));
    }

    // Message float, correct index, no unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::Float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1234., 0.));
    }

    // Message float, correct index, unit
    {
      // The unit is ignored since there is no unit in the address
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::Float(5678.), ossia::rgb_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // Message float, incorrect index, no unit
    {
      // The message is ignored
      ossia::message m{{*t.vec3f_addr, {12}}, ossia::Float(2222.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // Message float, incorrect index, unit
    {
      // The message is also ignored
      ossia::message m{{*t.vec3f_addr, {12}}, ossia::Float(2222.), ossia::rgb_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // TODO what about the case where the index is correct in the destination unit ?

    // Message vec, no index, no unit
    {
      ossia::message m{*t.vec3f_addr, ossia::make_vec(0., 1., 2.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1., 2.));
    }

    // Message vec, index, no unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(12., 1234., 27.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1234., 2.));
    }

    // Message vec, no index, unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(0., 1., 2.), ossia::rgb_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1., 2.));
    }

    // Message vec, index, unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(12., 1234., 27.), ossia::rgb_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1234., 2.));
    }

    /////// Address vec, unit ///////
    t.vec3f_addr->pushValue(ossia::make_vec(0., 0., 0.));
    t.vec3f_addr->setUnit(ossia::rgb_u{});

    // Message float, no index, no unit
    {
      // The message is ignored, we keep the previous value
      ossia::message m{*t.vec3f_addr, ossia::Float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 0., 0.));
    }

    // Message float, no index, unit
    {
      // The message is ignored, we keep the previous value
      ossia::message m{*t.vec3f_addr, ossia::Float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 0., 0.));
    }

    // Message float, index, no unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::Float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1234., 0.));
    }

    // Message float, index, same unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::Float(5678.), t.vec3f_addr->getUnit()};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // Message float, index, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {2}}, ossia::Float(0.7), ossia::hsv_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0.7, 0.7, 0.7));
    }

    // Message float, index, different dataspace
    {
      // Ignored
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {2}}, ossia::Float(0.7), ossia::axis_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0.5, 0.5, 0.5));
    }

    // Message vec, no index, no unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0.2, 0.3, 0.4));
    }

    // Message vec, no index, same unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), t.vec3f_addr->getUnit()};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0.2, 0.3, 0.4));
    }

    // Message vec, no index, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      // hsv{.2, .3, .4} == hsv{72°, 30%, 40%}
      ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), ossia::hsv_u{}};
      m.launch();
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.376, 0.4, 0.28)));
    }

    // Message vec, no index, different dataspace
    {
      // ignored
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), ossia::axis_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
    }


    // Message vec, index, no unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(0.2, 0.3, 0.4), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
    }

    // Message vec, index, same unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(0.2, 0.3, 0.4), t.vec3f_addr->getUnit()};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
    }

    // Message vec, index, different unit and same dataspace
    {
      // The whole value may be affected :
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(0.2, 0.3, 0.4), ossia::hsv_u{}};
      m.launch();
      // What is launched is a message looking like hsv{ 0., 0.3, 0.5 }:
      // - first the current color rgb{0.5, 0.5, 0.5} is converted to hsv{0., 0., 0.5}
      // - the value in the message is applied (here s = 0.3) : hsv{0, 0.3, 0.5}
      // - the *whole* vec is reconverted to rgb{0.5, 0.35, 0.35} and applied

      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.35, 0.35));
    }

    // Message vec, index, different dataspace
    {
      // Ignored
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {1}}, ossia::make_vec(0.2, 0.3, 0.4), ossia::axis_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
    }


    // Piecewise Vecf, 1 member, no unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{*t.vec3f_addr, ossia::make_vec(0.2, 0.3, 0.4), {}, make_bitset(false, true, false)};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
    }

    // Piecewise Vecf, 1 member, same unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            t.vec3f_addr->getUnit(),
            make_bitset(false, true, false)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.3, 0.5));
    }

    // Piecewise Vecf, 1 member, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            ossia::hsv_u{},
            make_bitset(false, true, false)};

      m.launch();
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.376, 0.4, 0.28)));
    }

    // Piecewise Vecf, 1 member, different dataspace
    {
      // ignore
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            ossia::axis_u{},
            make_bitset(false, true, false)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
    }


    // Piecewise Vecf, 2 member, no unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
        *t.vec3f_addr,
         ossia::make_vec(0.2, 0.3, 0.4),
         {},
         make_bitset(true, true, false)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.2, 0.3, 0.5));
    }

    // Piecewise Vecf, 2 member, same unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            t.vec3f_addr->getUnit(),
            make_bitset(true, true, false)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.2, 0.3, 0.5));
    }

    // Piecewise Vecf, 2 member, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            ossia::hsv_u{},
            make_bitset(true, true, false)};

      m.launch();
      debug(t.vec3f_addr->cloneValue());
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.471, 0.502, 0.349)));
    }

    // Piecewise Vecf, 2 member, different dataspace
    {
      // ignore
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            ossia::axis_u{},
            make_bitset(true, true, false)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
    }
    // Piecewise Vecf, all members, no unit
    // Piecewise Vecf, all members, same unit
    // Piecewise Vecf, all members, different unit and same dataspace
    // Piecewise Vecf, all members, different dataspace

  }


  void flatten_test()
  {

  }
};
QTEST_APPLESS_MAIN(DataspaceMergeTest)

#include "DataspaceMergeTest.moc"
