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
        auto curve = Curve<float>::create();
        QVERIFY(curve != nullptr);

        auto linearSegment = CurveSegmentLinear<float>::create(curve);

        curve->setInitialValue(0.);
        QVERIFY(curve->getInitialValue() == 0.);

        curve->addPoint(1., 1., linearSegment);
        QVERIFY(curve->getPointsMap().size() == 1);
        //QVERIFY(curve->segments().size() == 1);

        curve->addPoint(2., 0., linearSegment);
        QVERIFY(curve->getPointsMap().size() == 2);
        //QVERIFY(curve->segments().size() == 2);

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
        //QVERIFY(curve->segments().size() == 1);

        QVERIFY(curve->valueAt(0.) == 2.);
        QVERIFY(curve->valueAt(0.5) == 1.5);
        QVERIFY(curve->valueAt(1.) == 1.);
        QVERIFY(curve->valueAt(1.5) == 0.5);
        QVERIFY(curve->valueAt(2.) == 0.);

        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(CurveTest)

#include "CurveTest.moc"
