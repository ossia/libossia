#include <QtTest>
#include "../../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class CurveSegmentLinearTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions for bool type */
    void test_bool()
    {
        auto curve = Curve<bool>::create();
        auto curveSegment = CurveSegmentLinear<bool>::create(curve);
        QVERIFY(curveSegment != nullptr);

        QVERIFY(curveSegment->getType() == CurveSegment<bool>::Type::LINEAR);
        QVERIFY(curveSegment->getParent() == curve);

        QVERIFY(curveSegment->valueAt(0., false, true) == false);
        QVERIFY(curveSegment->valueAt(0.25, false, true) == true);
        QVERIFY(curveSegment->valueAt(0.5, false, true) == true);
        QVERIFY(curveSegment->valueAt(0.75, false, true) == true);
        QVERIFY(curveSegment->valueAt(1., false, true) == true);
    }

    /*! test life cycle and accessors functions for int type */
    void test_int()
    {
        auto curve = Curve<int>::create();
        auto curveSegment = CurveSegmentLinear<int>::create(curve);
        QVERIFY(curveSegment != nullptr);

        QVERIFY(curveSegment->getType() == CurveSegment<int>::Type::LINEAR);
        QVERIFY(curveSegment->getParent() == curve);

        QVERIFY(curveSegment->valueAt(0., 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(0.25, 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(0.5, 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(0.75, 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(1., 0, 1) == 1);
    }
    
    /*! test life cycle and accessors functions for float type */
    void test_float()
    {
        auto curve = Curve<float>::create();
        auto curveSegment = CurveSegmentLinear<float>::create(curve);
        QVERIFY(curveSegment != nullptr);

        QVERIFY(curveSegment->getType() == CurveSegment<float>::Type::LINEAR);
        QVERIFY(curveSegment->getParent() == curve);

        QVERIFY(curveSegment->valueAt(0., 0., 1.) == 0.);
        QVERIFY(curveSegment->valueAt(0.25, 0., 1.) == 0.25);
        QVERIFY(curveSegment->valueAt(0.5, 0., 1.) == 0.5);
        QVERIFY(curveSegment->valueAt(0.75, 0., 1.) == 0.75);
        QVERIFY(curveSegment->valueAt(1., 0., 1.) == 1.);
    }
};

QTEST_APPLESS_MAIN(CurveSegmentLinearTest)

#include "CurveSegmentLinearTest.moc"
