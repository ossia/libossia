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
        auto curve = Curve<double, float>::create();
        QVERIFY(curve != nullptr);

        auto linearSegment = CurveSegmentLinear<float>::create(curve);

        curve->setInitialValue(0.);
        QVERIFY(curve->getInitialValue() == 0.);

        curve->addPoint(linearSegment, 1., 1.);
        QVERIFY(curve->getPointsMap().size() == 1);

        curve->addPoint(linearSegment, 2., 0.);
        QVERIFY(curve->getPointsMap().size() == 2);

        QVERIFY(curve->valueAt(0.) == 0.);
        QVERIFY(curve->valueAt(0.5) == 0.5);
        QVERIFY(curve->valueAt(1.) == 1.);
        QVERIFY(curve->valueAt(1.5) == 0.5);
        QVERIFY(curve->valueAt(2.) == 0.);

        curve->setInitialValue(2.);
        QVERIFY(curve->getInitialValue() == 2.);

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

        //! \todo test clone()
    }
  
    void test_destination()
    {
        // Local device
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
      
        QVERIFY(curve->getInitialDestination() == &d);
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
