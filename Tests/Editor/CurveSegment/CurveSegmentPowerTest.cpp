#include <QtTest>
#include "../../ForwardDeclaration.h"
#include <iostream>

using namespace ossia;

class CurveSegmentPowerTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  /*! test life cycle and accessors functions for bool type */
  void test_bool()
  {
    {
      auto curveSegment = curve_segment_power<bool>{}(1.);
      QVERIFY(curveSegment(0., false, true) == false);
      QVERIFY(curveSegment(0.25, false, true) == true);
      QVERIFY(curveSegment(0.5, false, true) == true);
      QVERIFY(curveSegment(0.75, false, true) == true);
      QVERIFY(curveSegment(1., false, true) == true);
    }

    {
      auto curveSegment = curve_segment_power<bool>{}(1.);

      QVERIFY(curveSegment(0., false, true) == false);
      QVERIFY(curveSegment(0.25, false, true) == true);
      QVERIFY(curveSegment(0.5, false, true) == true);
      QVERIFY(curveSegment(0.75, false, true) == true);
      QVERIFY(curveSegment(1., false, true) == true);
    }
  }

  /*! test life cycle and accessors functions for int type */
  void test_int()
  {
    {
      auto curveSegment = curve_segment_power<int>{}(1.);
      QVERIFY(curveSegment(0., 0, 1) == 0);
      QVERIFY(curveSegment(0.25, 0, 1) == 0);
      QVERIFY(curveSegment(0.5, 0, 1) == 0);
      QVERIFY(curveSegment(0.75, 0, 1) == 0);
      QVERIFY(curveSegment(1., 0, 1) == 1);
    }

    {
      auto curveSegment = curve_segment_power<int>{}(2.);

      QVERIFY(curveSegment(0., 0, 1) == 0);
      QVERIFY(curveSegment(0.25, 0, 1) == 0);
      QVERIFY(curveSegment(0.5, 0, 1) == 0);
      QVERIFY(curveSegment(0.75, 0, 1) == 0);
      QVERIFY(curveSegment(1., 0, 1) == 1);
    }
  }

  /*! test life cycle and accessors functions for float type */
  void test_float()
  {
    {
      auto curveSegment = curve_segment_power<float>{}(1.);
      QVERIFY(curveSegment(0., 0., 1.) == 0.);
      QVERIFY(curveSegment(0.25, 0., 1.) == 0.25);
      QVERIFY(curveSegment(0.5, 0., 1.) == 0.5);
      QVERIFY(curveSegment(0.75, 0., 1.) == 0.75);
      QVERIFY(curveSegment(1., 0., 1.) == 1.);
    }

    {
      auto curveSegment = curve_segment_power<float>{}(1.);
      QVERIFY(curveSegment(0., 0., 1.) == 0.);
      QVERIFY(curveSegment(0.25, 0., 1.) == 0.0625);
      QVERIFY(curveSegment(0.5, 0., 1.) == 0.25);
      QVERIFY(curveSegment(0.75, 0., 1.) == 0.5625);
      QVERIFY(curveSegment(1., 0., 1.) == 1.);
    }
  }
};

QTEST_APPLESS_MAIN(CurveSegmentPowerTest)

#include "CurveSegmentPowerTest.moc"
