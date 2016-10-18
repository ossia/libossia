#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

class CurveTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:

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

    c->setInitialPointAbscissa(0.);
    QVERIFY(c->getInitialPointAbscissa() == 0.);

    c->setInitialPointOrdinate(0.);
    QVERIFY(c->getInitialPointOrdinate() == 0.);

    c->addPoint(linearSegment, 1., 1.);
    QVERIFY(c->getPointsMap().size() == 1);

    c->addPoint(linearSegment, 2., 0.);
    QVERIFY(c->getPointsMap().size() == 2);

    QVERIFY(c->valueAt(0.) == 0.);
    QVERIFY(c->valueAt(0.5) == 0.5);
    QVERIFY(c->valueAt(1.) == 1.);
    QVERIFY(c->valueAt(1.5) == 0.5);
    QVERIFY(c->valueAt(2.) == 0.);

    c->setInitialPointOrdinate(2.);
    QVERIFY(c->getInitialPointOrdinate() == 2.);

    QVERIFY(c->valueAt(0.) == 2.);
    QVERIFY(c->valueAt(0.5) == 1.5);
    QVERIFY(c->valueAt(1.) == 1.);
    QVERIFY(c->valueAt(1.5) == 0.5);
    QVERIFY(c->valueAt(2.) == 0.);

    c->removePoint(1.);
    QVERIFY(c->getPointsMap().size() == 1);

    QVERIFY(c->valueAt(0.) == 2.);
    QVERIFY(c->valueAt(0.5) == 1.5);
    QVERIFY(c->valueAt(1.) == 1.);
    QVERIFY(c->valueAt(1.5) == 0.5);
    QVERIFY(c->valueAt(2.) == 0.);
  }

  void test_float_float()
  {
    auto c = std::make_shared<curve<float, float>>();
    QVERIFY(c != nullptr);

    curve_segment_linear<float> linearSegment;

    c->setInitialPointAbscissa(-100.);
    QVERIFY(c->getInitialPointAbscissa() == -100.);

    c->setInitialPointOrdinate(-100.);
    QVERIFY(c->getInitialPointOrdinate() == -100.);

    c->addPoint(linearSegment, 0., 0.);
    QVERIFY(c->getPointsMap().size() == 1);

    QVERIFY(qFuzzyCompare(c->valueAt(-110.), float(-100.)));
    QVERIFY(qFuzzyCompare(c->valueAt(-100.), float(-100.)));
    QVERIFY(qFuzzyCompare(c->valueAt(-80.), float(-80.)));
    QVERIFY(qFuzzyCompare(c->valueAt(-60.), float(-60.)));
    QVERIFY(qFuzzyCompare(c->valueAt(-40.), float(-40.)));
    QVERIFY(qFuzzyCompare(c->valueAt(-20.), float(-20.)));
    QVERIFY(qFuzzyCompare(c->valueAt(-0.), float(0.)));
    QVERIFY(qFuzzyCompare(c->valueAt(10.), float(0.)));
  }

  void test_float_int()
  {
    auto c = std::make_shared<curve<float, int>>();
    curve_segment_linear<int> linearSegment;
    c->setInitialPointAbscissa(-10.);
    c->setInitialPointOrdinate(-10);
    c->addPoint(linearSegment, 10., 10);

    QVERIFY(c->valueAt(-10.) == -10);
    QCOMPARE(c->valueAt(-9.5), -9);
    QVERIFY(c->valueAt(-9.) == -9);
    QVERIFY(c->valueAt(-8.5) == -8);
    QVERIFY(c->valueAt(-8) == -8);
    QVERIFY(c->valueAt(-7.5) == -7);
    QVERIFY(c->valueAt(0.) == 0);
    QVERIFY(c->valueAt(10.) == 10);
  }

  void test_destination()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};

    auto localTupleNode = device.createChild("my_tuple");
    auto localTupleAddress = localTupleNode->createAddress(val_type::TUPLE);

    Tuple t{Float(-1.), Float(0.), Float(1.)};
    localTupleAddress->setValue(t);

    auto c = std::make_shared<curve<double, float>>();
    curve_segment_linear<float> linearSegment;

    c->setInitialPointAbscissa(0.);

    Destination d(*localTupleAddress, {1});
    c->setInitialPointOrdinateDestination(d);

    QVERIFY(c->getInitialPointOrdinateDestination() == d);

    c->addPoint(linearSegment, 1., 1.);

    QVERIFY(c->valueAt(0.) == 0.);
    QVERIFY(c->valueAt(0.5) == 0.5);
    QVERIFY(c->valueAt(1.) == 1.);
  }
};

QTEST_APPLESS_MAIN(CurveTest)

#include "CurveTest.moc"
