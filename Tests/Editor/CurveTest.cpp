#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class CurveTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        // <double, float> curve
        auto curveA = Curve<double, float>::create();
        QVERIFY(curveA != nullptr);

        auto linearSegmentA = CurveSegmentLinear<float>::create(curveA);

        curveA->setInitialPointAbscissa(0.);
        QVERIFY(curveA->getInitialPointAbscissa() == 0.);

        curveA->setInitialPointOrdinate(0.);
        QVERIFY(curveA->getInitialPointOrdinate() == 0.);

        curveA->addPoint(linearSegmentA, 1., 1.);
        QVERIFY(curveA->getPointsMap().size() == 1);

        curveA->addPoint(linearSegmentA, 2., 0.);
        QVERIFY(curveA->getPointsMap().size() == 2);

        QVERIFY(curveA->valueAt(0.) == 0.);
        QVERIFY(curveA->valueAt(0.5) == 0.5);
        QVERIFY(curveA->valueAt(1.) == 1.);
        QVERIFY(curveA->valueAt(1.5) == 0.5);
        QVERIFY(curveA->valueAt(2.) == 0.);

        curveA->setInitialPointOrdinate(2.);
        QVERIFY(curveA->getInitialPointOrdinate() == 2.);

        QVERIFY(curveA->valueAt(0.) == 2.);
        QVERIFY(curveA->valueAt(0.5) == 1.5);
        QVERIFY(curveA->valueAt(1.) == 1.);
        QVERIFY(curveA->valueAt(1.5) == 0.5);
        QVERIFY(curveA->valueAt(2.) == 0.);

        curveA->removePoint(1.);
        QVERIFY(curveA->getPointsMap().size() == 1);

        QVERIFY(curveA->valueAt(0.) == 2.);
        QVERIFY(curveA->valueAt(0.5) == 1.5);
        QVERIFY(curveA->valueAt(1.) == 1.);
        QVERIFY(curveA->valueAt(1.5) == 0.5);
        QVERIFY(curveA->valueAt(2.) == 0.);
        
        // <float, float> curve
        auto curveB = Curve<float, float>::create();
        QVERIFY(curveB != nullptr);
        
        auto linearSegmentB = CurveSegmentLinear<float>::create(curveB);
        
        curveB->setInitialPointAbscissa(-100.);
        QVERIFY(curveB->getInitialPointAbscissa() == -100.);

        curveB->setInitialPointOrdinate(-100.);
        QVERIFY(curveB->getInitialPointOrdinate() == -100.);
        
        curveB->addPoint(linearSegmentB, 0., 0.);
        QVERIFY(curveB->getPointsMap().size() == 1);
        
        QVERIFY(curveB->valueAt(-110.) == -100.);
        QVERIFY(curveB->valueAt(-100.) == -100.);
        QVERIFY(curveB->valueAt(-80.) == -80.);
        QVERIFY(curveB->valueAt(-60.) == -60.);
        //! \todo QVERIFY(curveB->valueAt(-40.) == -40.); //! \note it returns something like ~39.999... ?!?
        //! \todo QVERIFY(curveB->valueAt(-20.) == -20.); //! \note it returns something like ~19.999... ?!?
        QVERIFY(curveB->valueAt(0.) == 0.);
        QVERIFY(curveB->valueAt(10.) == 0.);

        //! \todo test clone()
    }
  
    void test_destination()
    {
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");
      
        auto localTupleNode = *(device->emplace(device->children().cend(), "my_tuple"));
        auto localTupleAddress = localTupleNode->createAddress(Value::Type::TUPLE);
      
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
