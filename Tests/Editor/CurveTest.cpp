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

        curveA->setInitialValue(0.);
        QVERIFY(curveA->getInitialValue() == 0.);

        curveA->addPoint(linearSegmentA, 1., 1.);
        QVERIFY(curveA->getPointsMap().size() == 1);

        curveA->addPoint(linearSegmentA, 2., 0.);
        QVERIFY(curveA->getPointsMap().size() == 2);

        QVERIFY(curveA->valueAt(0.) == 0.);
        QVERIFY(curveA->valueAt(0.5) == 0.5);
        QVERIFY(curveA->valueAt(1.) == 1.);
        QVERIFY(curveA->valueAt(1.5) == 0.5);
        QVERIFY(curveA->valueAt(2.) == 0.);

        curveA->setInitialValue(2.);
        QVERIFY(curveA->getInitialValue() == 2.);

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
        
        curveB->setInitialValue(-96.);
        QVERIFY(curveB->getInitialValue() == -96.);
        
        curveB->addPoint(linearSegmentB, 0., 0.);
        QVERIFY(curveB->getPointsMap().size() == 1);
        
        QVERIFY(curveB->valueAt(-100.) == -96.);
        QVERIFY(curveB->valueAt(-96.) == -96.);
        QVERIFY(curveB->valueAt(-80.) == -80.);
        QVERIFY(curveB->valueAt(-20.) == -20.);
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
      
        Destination d(localTupleNode);
        curve->setInitialDestination(&d);
        curve->setInitialDestinationIndex({1});
      
        QVERIFY(*curve->getInitialDestination() == d);
        QVERIFY(curve->getInitialDestinationIndex().size() == 1);
        QVERIFY(curve->getInitialDestinationIndex()[0] == 1);
      
        curve->addPoint(linearSegment, 1., 1.);
      
        QVERIFY(curve->valueAt(0.) == 0.);
        QVERIFY(curve->valueAt(0.5) == 0.5);
        QVERIFY(curve->valueAt(1.) == 1.);
    }
};

QTEST_APPLESS_MAIN(CurveTest)

#include "CurveTest.moc"
