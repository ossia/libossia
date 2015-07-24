#include <QtTest>
#include "../ForwardDeclaration.h"
#include <functional>
#include <iostream>

using namespace OSSIA;
using namespace std::placeholders;

class AutomationTest : public QObject
{
    Q_OBJECT

    std::vector<Value*> m_address_values;

    void automation_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<State> state)
    {
        state->launch();
    }

    void event_callback(TimeEvent::Status newStatus, TimeEvent::Status oldStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

    void address_callback(const Value * v)
    {
        m_address_values.push_back(v->clone());

        //! \debug
        Float* f = (Float*)v;
        std::cout << f->value << std::endl;
    }

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto callback = std::bind(&AutomationTest::automation_callback, this, _1, _2, _3);

        Local local_protocol{};
        auto local_device = Device::create(local_protocol, "test");
        local_device->emplace(local_device->children().begin(), "child");
        auto address = local_device->children().front()->createAddress(Value::Type::FLOAT);

        Float f(0);

        auto automation = Automation::create(callback, address, &f);
        QVERIFY(automation != nullptr);

        QVERIFY(automation->getStartState() != nullptr);
        QVERIFY(automation->getEndState() != nullptr);
        QVERIFY(automation->getClock() != nullptr);
        QVERIFY(automation->getParentTimeConstraint() == nullptr);

        QVERIFY(automation->getDrivenAddress() == address);
        QVERIFY(*automation->getDriving() == f);

        //! \todo test clone()
    }
    
    /*! test execution functions */
    //! \todo test stop()
    //! \todo test pause()
    //! \todo test resume()
    //! \todo test state()
    void test_execution()
    {
        auto automation_callback = std::bind(&AutomationTest::automation_callback, this, _1, _2, _3);

        Local local_protocol{};
        auto local_device = Device::create(local_protocol, "test");
        local_device->emplace(local_device->children().begin(), "child");
        auto address = local_device->children().front()->createAddress(Value::Type::FLOAT);
        auto address_callback = std::bind(&AutomationTest::address_callback, this, _1);
        address->setValueCallback(address_callback);

        auto curve = Curve<float>::create();
        auto linearSegment = CurveSegmentLinear<float>::create(curve);
        curve->setInitialValue(0.);
        curve->addPoint(0.5, 1., linearSegment);
        curve->addPoint(1., 0., linearSegment);
        Behavior b(curve);

        auto automation = Automation::create(automation_callback, address, &b);
        automation->getClock()->setGranularity(10.);

        auto start_node = TimeNode::create();
        auto end_node = TimeNode::create();
        auto event_callback = std::bind(&AutomationTest::event_callback, this, _1, _2);
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
        auto constraint = TimeConstraint::create(start_event, end_event, 100.);
        constraint->addTimeProcess(automation);

        m_address_values.clear();

        constraint->play();

        //! \todo check the running status of the constraint itself
        while (automation->getClock()->getRunning())
            ;

        QVERIFY(m_address_values.size() == 10);
        QVERIFY(m_address_values[9] == new Float(0.));
    }
};

QTEST_APPLESS_MAIN(AutomationTest)

#include "AutomationTest.moc"
