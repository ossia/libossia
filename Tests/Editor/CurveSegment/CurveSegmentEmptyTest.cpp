// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include "../../ForwardDeclaration.h"
#include <iostream>

using namespace ossia;

class CurveSegmentEmptyTest : public QObject
{
    Q_OBJECT

private:

    /*! test life cycle and accessors functions for bool type */
    Q_SLOT void test_bool()
    {
        curve_segment_empty<bool> curveSegment;

        QVERIFY(curveSegment(0., false, true) == false);
        QVERIFY(curveSegment(0.25, false, true) == false);
        QVERIFY(curveSegment(0.5, false, true) == false);
        QVERIFY(curveSegment(0.75, false, true) == false);
        QVERIFY(curveSegment(1., false, true) == false);
    }

    /*! test life cycle and accessors functions for int type */
    Q_SLOT void test_int()
    {
        curve_segment_empty<int> curveSegment;

        QVERIFY(curveSegment(0., 0, 1) == 0);
        QVERIFY(curveSegment(0.25, 0, 1) == 0);
        QVERIFY(curveSegment(0.5, 0, 1) == 0);
        QVERIFY(curveSegment(0.75, 0, 1) == 0);
        QVERIFY(curveSegment(1., 0, 1) == 0);
    }

    /*! test life cycle and accessors functions for float type */
    Q_SLOT void test_float()
    {
        curve_segment_empty<float> curveSegment;
        QVERIFY(curveSegment(0., 0., 1.) == 0.);
        QVERIFY(curveSegment(0.25, 0., 1.) == 0.);
        QVERIFY(curveSegment(0.5, 0., 1.) == 0.);
        QVERIFY(curveSegment(0.75, 0., 1.) == 0.);
        QVERIFY(curveSegment(1., 0., 1.) == 0.);
    }
};

QTEST_APPLESS_MAIN(CurveSegmentEmptyTest)

#include "CurveSegmentEmptyTest.moc"
