// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include "../../ForwardDeclaration.h"
#include <iostream>

using namespace ossia;

class CurveSegmentLinearTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  /*! test life cycle and accessors functions for bool type */
  void test_bool()
  {
    curve_segment_linear<bool> curveSegment;

    QVERIFY(curveSegment(0., false, true) == false);
    QVERIFY(curveSegment(0.25, false, true) == true);
    QVERIFY(curveSegment(0.5, false, true) == true);
    QVERIFY(curveSegment(0.75, false, true) == true);
    QVERIFY(curveSegment(1., false, true) == true);
  }

  /*! test life cycle and accessors functions for int type */
  void test_int()
  {
    curve_segment_linear<int> curveSegment;

    QVERIFY(curveSegment(0., 0, 1) == 0);
    QVERIFY(curveSegment(0.25, 0, 1) == 0);
    QVERIFY(curveSegment(0.5, 0, 1) == 0);
    QVERIFY(curveSegment(0.75, 0, 1) == 0);
    QVERIFY(curveSegment(1., 0, 1) == 1);
  }

  /*! test life cycle and accessors functions for float type */
  void test_float()
  {
    curve_segment_linear<float> curveSegment;

    QVERIFY(curveSegment(0., 0., 1.) == 0.);
    QVERIFY(curveSegment(0.25, 0., 1.) == 0.25);
    QVERIFY(curveSegment(0.5, 0., 1.) == 0.5);
    QVERIFY(curveSegment(0.75, 0., 1.) == 0.75);
    QVERIFY(curveSegment(1., 0., 1.) == 1.);
  }
};

QTEST_APPLESS_MAIN(CurveSegmentLinearTest)

#include "CurveSegmentLinearTest.moc"
