#include <QtTest>
#include "../../ForwardDeclaration.h"
#include <iostream>

using namespace ossia;

class CurveSegmentEmptyTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions for bool type */
    void test_bool()
    {
        auto curve = Curve<double, bool>::create();
        auto curveSegment = CurveSegmentEmpty<bool>::create(curve);
        QVERIFY(curveSegment != nullptr);

        QVERIFY(curveSegment->getType() == CurveSegment<bool>::Type::EMPTY);
        QVERIFY(curveSegment->getParent() == curve);

        QVERIFY(curveSegment->valueAt(0., false, true) == false);
        QVERIFY(curveSegment->valueAt(0.25, false, true) == false);
        QVERIFY(curveSegment->valueAt(0.5, false, true) == false);
        QVERIFY(curveSegment->valueAt(0.75, false, true) == false);
        QVERIFY(curveSegment->valueAt(1., false, true) == false);

        //! \todo test clone()
    }

    /*! test life cycle and accessors functions for int type */
    void test_int()
    {
        auto curve = Curve<double, int>::create();
        auto curveSegment = CurveSegmentEmpty<int>::create(curve);
        QVERIFY(curveSegment != nullptr);

        QVERIFY(curveSegment->getType() == CurveSegment<int>::Type::EMPTY);
        QVERIFY(curveSegment->getParent() == curve);

        QVERIFY(curveSegment->valueAt(0., 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(0.25, 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(0.5, 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(0.75, 0, 1) == 0);
        QVERIFY(curveSegment->valueAt(1., 0, 1) == 0);

        //! \todo test clone()
    }
    
    /*! test life cycle and accessors functions for float type */
    void test_float()
    {
        auto curve = Curve<double, float>::create();
        auto curveSegment = CurveSegmentEmpty<float>::create(curve);
        QVERIFY(curveSegment != nullptr);

        QVERIFY(curveSegment->getType() == CurveSegment<float>::Type::EMPTY);
        QVERIFY(curveSegment->getParent() == curve);

        QVERIFY(curveSegment->valueAt(0., 0., 1.) == 0.);
        QVERIFY(curveSegment->valueAt(0.25, 0., 1.) == 0.);
        QVERIFY(curveSegment->valueAt(0.5, 0., 1.) == 0.);
        QVERIFY(curveSegment->valueAt(0.75, 0., 1.) == 0.);
        QVERIFY(curveSegment->valueAt(1., 0., 1.) == 0.);

        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(CurveSegmentEmptyTest)

#include "CurveSegmentEmptyTest.moc"
