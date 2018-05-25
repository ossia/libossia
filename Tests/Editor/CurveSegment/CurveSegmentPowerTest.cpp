// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <ossia/editor/curve/curve_segment/power.hpp>
#include <QtTest>
#include <iostream>

using namespace ossia;

class CurveSegmentPowerTest : public QObject
{
  Q_OBJECT

private:

  /*! test life cycle and accessors functions for bool type */
  Q_SLOT void test_bool()
  {
    {
      auto curveSegment = curve_segment_power<bool>{}(1.);
      QCOMPARE(curveSegment(0., false, true), false);
      QCOMPARE(curveSegment(0.25, false, true), true);
      QCOMPARE(curveSegment(0.5, false, true), true);
      QCOMPARE(curveSegment(0.75, false, true), true);
      QCOMPARE(curveSegment(1., false, true), true);
    }

    {
      auto curveSegment = curve_segment_power<bool>{}(2.);

      QCOMPARE(curveSegment(0., false, true), false);
      QCOMPARE(curveSegment(0.25, false, true), true);
      QCOMPARE(curveSegment(0.5, false, true), true);
      QCOMPARE(curveSegment(0.75, false, true), true);
      QCOMPARE(curveSegment(1., false, true), true);
    }
  }

  /*! test life cycle and accessors functions for int type */
  Q_SLOT void test_int()
  {
    {
      auto curveSegment = curve_segment_power<int>{}(1.);
      QCOMPARE(curveSegment(0., 0, 1), 0);
      QCOMPARE(curveSegment(0.25, 0, 1), 0);
      QCOMPARE(curveSegment(0.5, 0, 1), 0);
      QCOMPARE(curveSegment(0.75, 0, 1), 0);
      QCOMPARE(curveSegment(1., 0, 1), 1);
    }

    {
      auto curveSegment = curve_segment_power<int>{}(2.);

      QCOMPARE(curveSegment(0., 0, 1), 0);
      QCOMPARE(curveSegment(0.25, 0, 1), 0);
      QCOMPARE(curveSegment(0.5, 0, 1), 0);
      QCOMPARE(curveSegment(0.75, 0, 1), 0);
      QCOMPARE(curveSegment(1., 0, 1), 1);
    }
  }

  /*! test life cycle and accessors functions for float type */
  Q_SLOT void test_float()
  {
    {
      auto curveSegment = curve_segment_power<float>{}(1.);
      QCOMPARE(curveSegment(0., 0., 1.), 0.);
      QCOMPARE(curveSegment(0.25, 0., 1.), 0.25);
      QCOMPARE(curveSegment(0.5, 0., 1.), 0.5);
      QCOMPARE(curveSegment(0.75, 0., 1.), 0.75);
      QCOMPARE(curveSegment(1., 0., 1.), 1.);
    }

    {
      auto curveSegment = curve_segment_power<float>{}(2.);
      QCOMPARE(curveSegment(0., 0., 1.), 0.);
      QCOMPARE(curveSegment(0.25, 0., 1.), 0.0625);
      QCOMPARE(curveSegment(0.5, 0., 1.), 0.25);
      QCOMPARE(curveSegment(0.75, 0., 1.), 0.5625);
      QCOMPARE(curveSegment(1., 0., 1.), 1.);
    }
  }
};

QTEST_APPLESS_MAIN(CurveSegmentPowerTest)

#include "CurveSegmentPowerTest.moc"
