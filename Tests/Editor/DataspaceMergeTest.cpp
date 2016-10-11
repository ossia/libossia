#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <QtTest>
#include "TestUtils.hpp"
class DataspaceMergeTest : public QObject
{
  Q_OBJECT

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
      // The message is also ignored
      ossia::message m{*t.vec3f_addr, ossia::make_vec(0., 1., 2.), {}};
      m.launch();
      QVERIFY(t.vec3f_addr->cloneValue() == ossia::make_vec(0., 1., 2.));
    }
    // Message vec, index, no unit
    // Message vec, no index, unit
    // Message vec, index, unit

    /////// Address vec, unit ///////
    // Message float, no index
    // Message float, index, no unit
    // Message float, index, same unit
    // Message float, index, different unit and same dataspace
    // Message float, index, different dataspace

    // Message vec, no index, no unit
    // Message vec, no index, same unit
    // Message vec, no index, different unit and same dataspace
    // Message vec, no index, different dataspace

    // Message vec, index, no unit
    // Message vec, index, same unit
    // Message vec, index, different unit and same dataspace
    // Message vec, index, different dataspace

    // Piecewise Vecf, 1 member, no unit
    // Piecewise Vecf, 1 member, same unit
    // Piecewise Vecf, 1 member, different unit and same dataspace
    // Piecewise Vecf, 1 member, different dataspace

    // Piecewise Vecf, all members, no unit
    // Piecewise Vecf, all members, same unit
    // Piecewise Vecf, all members, different unit and same dataspace
    // Piecewise Vecf, all members, different dataspace

  }
};
QTEST_APPLESS_MAIN(DataspaceMergeTest)

#include "DataspaceMergeTest.moc"
