// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

class CurveTest : public QObject
{
  Q_OBJECT

private:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    //! \todo test clone()
  }

  void test_double_float()
  {
    // <double, float> curve
    auto c = std::make_shared<curve<double, float>>();
    QVERIFY(c != nullptr);

    curve_segment_linear<float> linearSegment;

    c->set_x0(0.);
    QVERIFY(c->get_x0() == 0.);

    c->set_y0(0.);
    QVERIFY(c->get_y0() == 0.);

    c->add_point(linearSegment, 1., 1.);
    QVERIFY(c->get_points().size() == 1);

    c->add_point(linearSegment, 2., 0.);
    QVERIFY(c->get_points().size() == 2);

    QVERIFY(c->value_at(0.) == 0.);
    QVERIFY(c->value_at(0.5) == 0.5);
    QVERIFY(c->value_at(1.) == 1.);
    QVERIFY(c->value_at(1.5) == 0.5);
    QVERIFY(c->value_at(2.) == 0.);

    c->set_y0(2.);
    QVERIFY(c->get_y0() == 2.);

    QVERIFY(c->value_at(0.) == 2.);
    QVERIFY(c->value_at(0.5) == 1.5);
    QVERIFY(c->value_at(1.) == 1.);
    QVERIFY(c->value_at(1.5) == 0.5);
    QVERIFY(c->value_at(2.) == 0.);

    c->remove_point(1.);
    QVERIFY(c->get_points().size() == 1);

    QVERIFY(c->value_at(0.) == 2.);
    QVERIFY(c->value_at(0.5) == 1.5);
    QVERIFY(c->value_at(1.) == 1.);
    QVERIFY(c->value_at(1.5) == 0.5);
    QVERIFY(c->value_at(2.) == 0.);
  }

  void test_float_float()
  {
    auto c = std::make_shared<curve<float, float>>();
    QVERIFY(c != nullptr);

    curve_segment_linear<float> linearSegment;

    c->set_x0(-100.);
    QVERIFY(c->get_x0() == -100.);

    c->set_y0(-100.);
    QVERIFY(c->get_y0() == -100.);

    c->add_point(linearSegment, 0., 0.);
    QVERIFY(c->get_points().size() == 1);

    QVERIFY(qFuzzyCompare(c->value_at(-110.), float(-100.)));
    QVERIFY(qFuzzyCompare(c->value_at(-100.), float(-100.)));
    QVERIFY(qFuzzyCompare(c->value_at(-80.), float(-80.)));
    QVERIFY(qFuzzyCompare(c->value_at(-60.), float(-60.)));
    QVERIFY(qFuzzyCompare(c->value_at(-40.), float(-40.)));
    QVERIFY(qFuzzyCompare(c->value_at(-20.), float(-20.)));
    QVERIFY(qFuzzyCompare(c->value_at(-0.), float(0.)));
    QVERIFY(qFuzzyCompare(c->value_at(10.), float(0.)));
  }

  void test_float_int()
  {
    auto c = std::make_shared<curve<float, int>>();
    curve_segment_linear<int> linearSegment;
    c->set_x0(-10.);
    c->set_y0(-10);
    c->add_point(linearSegment, 10., 10);

    QVERIFY(c->value_at(-10.) == -10);
    QCOMPARE(c->value_at(-9.5), -9);
    QVERIFY(c->value_at(-9.) == -9);
    QVERIFY(c->value_at(-8.5) == -8);
    QVERIFY(c->value_at(-8) == -8);
    QVERIFY(c->value_at(-7.5) == -7);
    QVERIFY(c->value_at(0.) == 0);
    QVERIFY(c->value_at(10.) == 10);
  }

  void test_destination()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    auto localTupleNode = device.create_child("my_tuple");
    auto localTupleAddress = localTupleNode->create_parameter(val_type::LIST);

    std::vector<ossia::value> t{float{-1.}, float{0.}, float{1.}};
    localTupleAddress->set_value(t);

    auto c = std::make_shared<curve<double, float>>();
    curve_segment_linear<float> linearSegment;

    c->set_x0(0.);

    destination d(*localTupleAddress, ossia::destination_index{1});
    c->set_y0_destination(d);

    QVERIFY(c->get_y0_destination() == d);

    c->add_point(linearSegment, 1., 1.);

    QVERIFY(c->value_at(0.) == 0.);
    QVERIFY(c->value_at(0.5) == 0.5);
    QVERIFY(c->value_at(1.) == 1.);
  }
};

QTEST_APPLESS_MAIN(CurveTest)

#include "CurveTest.moc"
