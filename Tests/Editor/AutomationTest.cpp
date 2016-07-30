#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <functional>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class AutomationTest : public QObject
{
    Q_OBJECT

    std::vector<Value*> m_address_values;

    void constraint_callback(time_value position, time_value date, std::shared_ptr<StateElement> element)
    {
        element->launch();
    }

    void event_callback(time_event::Status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

    void address_callback(const Value * v)
    {
        m_address_values.push_back(v->clone());
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");
        local_device->emplace(local_device->children().begin(), "child");
        auto address = local_device->children().front()->createAddress(Type::FLOAT);

        Float f(0);

        auto automation = automation::create(address, &f);
        QVERIFY(automation != nullptr);

        QVERIFY(automation->parent == nullptr);

        QVERIFY(automation->getDrivenAddress() == address);
        QVERIFY(*automation->getDriving() == f);

        //! \todo test clone()
    }

    /*! test execution functions */
    //! \todo test state()
    void test_execution()
    {
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");
        local_device->emplace(local_device->children().begin(), "child");
        auto address = local_device->children().front()->createAddress(Type::FLOAT);
        address->addCallback([&] (const Value* v) { address_callback(v); });

        auto curve = Curve<double, float>::create();
        auto linearSegment = CurveSegmentLinear<float>::create(curve);
      
        curve->setInitialPointAbscissa(0.);
        curve->setInitialPointOrdinate(0.);
        curve->addPoint(linearSegment, 0.5, 1.);
        curve->addPoint(linearSegment, 1., 0.);
        Behavior b(curve);
        auto automation = automation::create(address, &b);

        auto start_node = time_node::create();
        auto end_node = time_node::create();
        auto event_callback = std::bind(&AutomationTest::event_callback, this, _1);
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
        auto constraint_callback = std::bind(&AutomationTest::constraint_callback, this, _1, _2, _3);
        auto constraint = time_constraint::create(constraint_callback, start_event, end_event, 100., 100., 100.);
        constraint->addTimeProcess(automation);

        m_address_values.clear();

        constraint->setGranularity(10.);
        constraint->start();

        while (constraint->getRunning())
            ;

        QVERIFY(m_address_values.size() == 11);

        Float zero(0);
        QVERIFY(*m_address_values[0] == zero);
        QVERIFY(*m_address_values[10] == zero);

        // check if all values are differents from the previous value
        Value* previous = new Float(-1);
        bool different_from_previous;
        for (auto v : m_address_values)
        {
            different_from_previous = *v != *previous;
            if (!different_from_previous)
                break;
            previous = v;
        }
        QVERIFY(different_from_previous);
    }
};

QTEST_APPLESS_MAIN(AutomationTest)

#include "AutomationTest.moc"
