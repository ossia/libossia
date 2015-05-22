#include <QtTest>

#include "Editor/Curve.h"
#include "Editor/Automation.h"
#include "Editor/Scenario.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;

class TimeEventTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCase1()
    {
        auto scenar = Scenario::create();
        auto startTimeNode = TimeNode::create();
        auto endTimeNode = TimeNode::create();
        TimeValue d;
        auto cstr = TimeConstraint::create(d,d,d);
        auto autom = Automation<double>::create();

        scenar->addConstraint(*cstr, *startTimeNode, *endTimeNode);
        cstr->timeProcesses().insert(cstr->timeProcesses().begin(), autom);

        auto startEv = autom->getStartEvent();
        auto endEv = autom->getEndEvent();
/*
        QVERIFY(startEv->getNextProcesses().size() == 1);
        QVERIFY(endEv->getPreviousProcesses().size() == 1);
        QCOMPARE(startEv->getState(), autom->getStartState());
        QCOMPARE(endEv->getState(), autom->getEndState());
        startEv->getExpression();
*/
    }
};


QTEST_APPLESS_MAIN(TimeEventTest)

#include "TimeEventTest.moc"
