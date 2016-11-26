#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <ossia/editor/dataspace/dataspace_parse.hpp>
#include <QtTest>
#include "TestUtils.hpp"
class DataspaceMergeTest : public QObject
{
  Q_OBJECT

  void debug(const ossia::value& t) { qDebug() << ossia::value_to_pretty_string(t).c_str(); }

  template<std::size_t N>
  bool fuzzy_compare(std::array<float, N> v1, std::array<float, N> v2)
  {
    bool b = true;
    double eps = 1e-6;
    for(std::size_t i = 0; i < N; i++)
    {
      if(std::abs(v1[i] - v2[i]) > eps)
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
    static_assert(ossia::detail::is_array<ossia::Vec3f>::value, "");
    static_assert(ossia::detail::is_array<const ossia::Vec3f>::value, "");
    static_assert(!ossia::detail::is_array<float>::value, "");
    static_assert(!ossia::detail::is_array<const float>::value, "");

    static_assert(!ossia::detail::is_array<decltype(ossia::centimeter::dataspace_value)>::value, "");
    static_assert(ossia::detail::is_array<decltype(ossia::rgb::dataspace_value)>::value, "RGB is not iterable");

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
    t.float_addr->pushValue(float(0.));

    // Message float, no unit
    {
      float expected(3.2);
      ossia::message m{*t.float_addr, expected};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }

    // Message float, unit
    {
      float expected(3.2);
      ossia::message m{{*t.float_addr, ossia::meter_per_second_u{}}, expected};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }


    /////// Address float, unit ///////
    t.float_addr->setUnit(ossia::meter_per_second_u{});

    // Message float, no unit
    {
      float expected(3.2);
      ossia::message m{*t.float_addr, expected};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }

    // Message float, same unit
    {
      float expected(3.2);
      ossia::message m{{*t.float_addr, ossia::meter_per_second_u{}}, expected};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == expected);
    }

    // Message float, different unit and same dataspace
    {
      ossia::message m{{*t.float_addr, ossia::kilometer_per_hour_u{}}, float(1.)};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == float(1. / 3.6)); // 1km/h -> 0.27777 m/s
    }

    // Message float, different dataspace
    {
      // The message is ignored, we keep the previous value
      ossia::message m{{*t.float_addr, ossia::meter_u{}}, float(1234.)};
      m.launch();
      QVERIFY(t.float_addr->cloneValue() == float(1. / 3.6));
    }


    /////// Address vec, no unit ///////
    t.vec3f_addr->pushValue(ossia::make_vec(0., 0., 0.));

    // Message float, no index, no unit
    {
      ossia::message m{*t.vec3f_addr, float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(1234., 1234., 1234.));
    }

    t.vec3f_addr->pushValue(ossia::make_vec(0., 0., 0.));
    // Message float, no index, unit
    {
      // The message is ignored, we keep the previous value
      ossia::message m{*t.vec3f_addr, float(1234.), ossia::rgb_u{}}; // shouldn't be possible
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 0., 0.));
    }

    // Message float, correct index, no unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1234., 0.));
    }

    // Message float, correct index, unit
    {
      // The unit is ignored since there is no unit in the address
      ossia::message m{{*t.vec3f_addr, {1}}, float(5678.), ossia::rgb_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // Message float, incorrect index, no unit
    {
      // The message is ignored
      ossia::message m{{*t.vec3f_addr, {12}}, float(2222.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // Message float, incorrect index, unit
    {
      // The message is also ignored
      ossia::message m{{*t.vec3f_addr, {12}}, float(2222.), ossia::rgb_u{}};
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
      ossia::message m{*t.vec3f_addr, float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(1234., 1234., 1234.));
    }

    t.vec3f_addr->pushValue(ossia::make_vec(0., 0., 0.));
    // Message float, no index, unit
    {
      ossia::message m{*t.vec3f_addr, float(1234.), t.vec3f_addr->getUnit()};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(1234., 1234., 1234.));
    }

    t.vec3f_addr->pushValue(ossia::make_vec(0., 0., 0.));
    // Message float, index, no unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, float(1234.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1234., 0.));
    }

    // Message float, index, same unit
    {
      ossia::message m{{*t.vec3f_addr, {1}}, float(5678.), t.vec3f_addr->getUnit()};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 5678., 0.));
    }

    // Message float, index, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {2}}, float(0.7), ossia::hsv_u{}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0.7, 0.7, 0.7));
    }

    // Message float, index, different dataspace
    {
      // Ignored
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::message m{{*t.vec3f_addr, {2}}, float(0.7), ossia::axis_u{}};
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
      // rgb(0.5, 0.5, 0.5)
      // hsv(0, 0, 0.5)
      // hsv(0, 0.3, 0.5)
      // rgb(0.5, 0.35, 0.35)

      m.launch();
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.5, 0.35, 0.35)));
    }

    // Piecewise Vecf, 1 member, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(1., 0., 0.));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.5, 0., 0.),
            ossia::hsv_u{},
            make_bitset(true, false, false)};
      // rgb(1, 0, 0)
      // hsv(0, 1, 1)
      // hsv(0.5, 1, 1)
      // rgb(0, 1, 1)

      m.launch();
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.0, 1., 1.)));
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
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.47, 0.5, 0.35)));
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
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
        *t.vec3f_addr,
         ossia::make_vec(0.2, 0.3, 0.4),
         {},
         make_bitset(true, true, true)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.2, 0.3, 0.4));
    }

    // Piecewise Vecf, all members, same unit
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            t.vec3f_addr->getUnit(),
            make_bitset(true, true, true)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.2, 0.3, 0.4));
    }

    // Piecewise Vecf, all members, different unit and same dataspace
    {
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            ossia::hsv_u{},
            make_bitset(true, true, true)};

      m.launch();
      debug(t.vec3f_addr->cloneValue());
      QVERIFY(fuzzy_compare(t.vec3f_addr->cloneValue().get<ossia::Vec3f>(), ossia::make_vec(0.376, 0.40, 0.28)));
    }

    // Piecewise Vecf, all members, different dataspace
    {
      // ignore
      t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));
      ossia::piecewise_vec_message<3> m{
           *t.vec3f_addr,
            ossia::make_vec(0.2, 0.3, 0.4),
            ossia::axis_u{},
            make_bitset(true, true, true)};

      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue().get<ossia::Vec3f>() == ossia::make_vec(0.5, 0.5, 0.5));
    }
  }

  void flatten_same_vec_message_on_vec_address()
  {
    ossia::TestUtils t;
    t.vec3f_addr->setUnit(ossia::rgb_u{});
    t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));


    { // cref
      ossia::state s;

      ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);
    }

    { // rvalue
      ossia::state s;

      ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);
    }
  }


  void flatten_different_vec_message_on_vec_address()
  {
    ossia::TestUtils t;
    t.vec3f_addr->setUnit(ossia::rgb_u{});
    t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));

    { // cref
      ossia::state s;

      ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};
      ossia::message m2{*t.vec3f_addr, ossia::make_vec(0.4, 0.5, 0.6), {}};

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, m2);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m2);
    }

    { // rvalue
      ossia::state s;

      ossia::message m1{*t.vec3f_addr, ossia::make_vec(0.1, 0.2, 0.3), {}};
      ossia::message m2{*t.vec3f_addr, ossia::make_vec(0.4, 0.5, 0.6), {}};

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, ossia::message{m2});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m2);
    }
  }

  void flatten_different_float_message_on_vec_address_without_unit()
  {
    ossia::TestUtils t;
    t.vec3f_addr->setUnit(ossia::rgb_u{});
    t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));

    { // cref
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, {}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, {}};

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, m2);

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }

    { // rvalue
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, {}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, {}};

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, ossia::message{m2});

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }

    // Same but without an unit on the address
    t.vec3f_addr->setUnit({});
    t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));

    { // cref
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, {}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, {}};

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, m2);

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }

    { // rvalue
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, {}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, {}};

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, ossia::message{m2});

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), {}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }
  }


  void flatten_different_float_message_on_vec_address_with_same_unit()
  {
    ossia::TestUtils t;
    t.vec3f_addr->setUnit(ossia::rgb_u{});
    t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));

    { // cref
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, ossia::rgb_u{}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, ossia::rgb_u{}};

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, m2);

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::rgb_u{}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }

    { // rvalue
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, ossia::rgb_u{}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, ossia::rgb_u{}};

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, ossia::message{m2});

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::rgb_u{}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }

  }

  void flatten_different_float_message_on_vec_address_with_different_unit()
  {
    ossia::TestUtils t;
    t.vec3f_addr->setUnit(ossia::rgb_u{});
    t.vec3f_addr->pushValue(ossia::make_vec(0.5, 0.5, 0.5));

    { // cref
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, ossia::hsv_u{}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, ossia::hsv_u{}};

      ossia::flatten_and_filter(s, m1);

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, m2);

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::hsv_u{}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }

    { // rvalue
      ossia::state s;

      ossia::message m1{{*t.vec3f_addr, {0}}, float{1.}, ossia::hsv_u{}};
      ossia::message m2{{*t.vec3f_addr, {2}}, float{5.}, ossia::hsv_u{}};

      ossia::flatten_and_filter(s, ossia::message{m1});

      QVERIFY(s.size() == 1);
      QVERIFY(*s.begin() == m1);

      ossia::flatten_and_filter(s, ossia::message{m2});

      QVERIFY(s.size() == 1);
      ossia::piecewise_vec_message<3> expected{*t.vec3f_addr, ossia::make_vec(1., 0., 5.), ossia::hsv_u{}, make_bitset(true, false, true)};
      QVERIFY(*s.begin() == expected);
    }
  }

  void flatten_same_tuple_message_on_tuple_address()
  {
      ossia::TestUtils t;
      t.tuple_addr->setUnit({});

      { // cref
        ossia::state s;

        ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, {}};

        ossia::flatten_and_filter(s, m1);

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);

        ossia::flatten_and_filter(s, m1);

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);
      }

      { // rvalue
        ossia::state s;

        ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, {}};

        ossia::flatten_and_filter(s, ossia::message{m1});

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);

        ossia::flatten_and_filter(s, ossia::message{m1});

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);
      }


      t.tuple_addr->setUnit(ossia::rgb_u{});

      { // cref
        ossia::state s;

        ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, ossia::rgb_u{}};

        ossia::flatten_and_filter(s, m1);

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);

        ossia::flatten_and_filter(s, m1);

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);
      }

      { // rvalue
        ossia::state s;

        ossia::message m1{*t.tuple_addr, std::vector<ossia::value>{0., 0.5, 0.2}, ossia::rgb_u{}};

        ossia::flatten_and_filter(s, ossia::message{m1});

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);

        ossia::flatten_and_filter(s, ossia::message{m1});

        QVERIFY(s.size() == 1);
        QVERIFY(*s.begin() == m1);
      }

  }

};
QTEST_APPLESS_MAIN(DataspaceMergeTest)

#include "DataspaceMergeTest.moc"
