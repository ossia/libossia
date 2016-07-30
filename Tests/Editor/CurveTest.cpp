#include <QtTest>
#include <ossia/OSSIA.hpp>
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
        auto curve = Curve<double, float>::create();
        QVERIFY(curve != nullptr);

        auto linearSegment = CurveSegmentLinear<float>::create(curve);

        curve->setInitialPointAbscissa(0.);
        QVERIFY(curve->getInitialPointAbscissa() == 0.);

        curve->setInitialPointOrdinate(0.);
        QVERIFY(curve->getInitialPointOrdinate() == 0.);

        curve->addPoint(linearSegment, 1., 1.);
        QVERIFY(curve->getPointsMap().size() == 1);

        curve->addPoint(linearSegment, 2., 0.);
        QVERIFY(curve->getPointsMap().size() == 2);

        QVERIFY(curve->valueAt(0.) == 0.);
        QVERIFY(curve->valueAt(0.5) == 0.5);
        QVERIFY(curve->valueAt(1.) == 1.);
        QVERIFY(curve->valueAt(1.5) == 0.5);
        QVERIFY(curve->valueAt(2.) == 0.);

        curve->setInitialPointOrdinate(2.);
        QVERIFY(curve->getInitialPointOrdinate() == 2.);

        QVERIFY(curve->valueAt(0.) == 2.);
        QVERIFY(curve->valueAt(0.5) == 1.5);
        QVERIFY(curve->valueAt(1.) == 1.);
        QVERIFY(curve->valueAt(1.5) == 0.5);
        QVERIFY(curve->valueAt(2.) == 0.);

        curve->removePoint(1.);
        QVERIFY(curve->getPointsMap().size() == 1);

        QVERIFY(curve->valueAt(0.) == 2.);
        QVERIFY(curve->valueAt(0.5) == 1.5);
        QVERIFY(curve->valueAt(1.) == 1.);
        QVERIFY(curve->valueAt(1.5) == 0.5);
        QVERIFY(curve->valueAt(2.) == 0.);
    }

    void test_float_float()
    {
        auto curve = Curve<float, float>::create();
        QVERIFY(curve != nullptr);
        
        auto linearSegment = CurveSegmentLinear<float>::create(curve);
        
        curve->setInitialPointAbscissa(-100.);
        QVERIFY(curve->getInitialPointAbscissa() == -100.);

        curve->setInitialPointOrdinate(-100.);
        QVERIFY(curve->getInitialPointOrdinate() == -100.);
        
        curve->addPoint(linearSegment, 0., 0.);
        QVERIFY(curve->getPointsMap().size() == 1);

        QVERIFY(qFuzzyCompare(curve->valueAt(-110.), float(-100.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(-100.), float(-100.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(-80.), float(-80.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(-60.), float(-60.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(-40.), float(-40.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(-20.), float(-20.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(-0.), float(0.)));
        QVERIFY(qFuzzyCompare(curve->valueAt(10.), float(0.)));
    }

    void test_float_int()
    {
        auto curve = Curve<float, int>::create();
        auto linearSegment = CurveSegmentLinear<int>::create(curve);
        curve->setInitialPointAbscissa(-10.);
        curve->setInitialPointOrdinate(-10);
        curve->addPoint(linearSegment, 10., 10);

        QVERIFY(curve->valueAt(-10.) == -10);
        QVERIFY(curve->valueAt(-9.5) == -9);
        QVERIFY(curve->valueAt(-9.) == -9);
        QVERIFY(curve->valueAt(-8.5) == -8);
        QVERIFY(curve->valueAt(-8) == -8);
        QVERIFY(curve->valueAt(-7.5) == -7);
        QVERIFY(curve->valueAt(0.) == 0);
        QVERIFY(curve->valueAt(10.) == 10);
    }
  
    void test_destination()
    {
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");
      
        auto localTupleNode = *(device->emplace(device->children().cend(), "my_tuple"));
        auto localTupleAddress = localTupleNode->createAddress(Type::TUPLE);
      
        Tuple t = {new Float(-1.), new Float(0.), new Float(1.)};
        localTupleAddress->setValue(&t);
      
        auto curve = Curve<double, float>::create();
        auto linearSegment = CurveSegmentLinear<float>::create(curve);
      
        curve->setInitialPointAbscissa(0.);
      
        Destination d(localTupleNode, {1});
        curve->setInitialPointOrdinateDestination(&d);
      
        QVERIFY(*curve->getInitialPointOrdinateDestination() == d);

        curve->addPoint(linearSegment, 1., 1.);
      
        QVERIFY(curve->valueAt(0.) == 0.);
        QVERIFY(curve->valueAt(0.5) == 0.5);
        QVERIFY(curve->valueAt(1.) == 1.);
    }
};

QTEST_APPLESS_MAIN(CurveTest)

#include "CurveTest.moc"
