#include <QtTest>

#include "../Sources/Editor/Curve.cpp"
#include "../Sources/Editor/Automation.cpp"
#include "../Sources/Editor/Scenario.cpp"
#include "../Sources/Editor/TimeConstraint.cpp"
#include "../Sources/Editor/TimeNode.cpp"
#include "../Sources/Editor/TimeEvent.cpp"
#include "../Sources/Editor/TimeValue.cpp"


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
        cstr->insert(cstr->begin(), autom);
    }
};


QTEST_APPLESS_MAIN(TimeEventTest)

#include "TimeEventTest.moc"
