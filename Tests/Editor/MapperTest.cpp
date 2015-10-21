#include <QtTest>
#include "../ForwardDeclaration.h"
#include <functional>
#include <thread>
#include <iostream>

using namespace OSSIA;
using namespace std::chrono;
using namespace std::placeholders;

class MapperTest : public QObject
{
    Q_OBJECT

    std::vector<Value*> m_float_address_values;
    std::vector<Value*> m_int_address_values;

    void constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> element)
    {
        element->launch();
    }

    void event_callback(TimeEvent::Status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

    void float_address_callback(const Value * v)
    {
        m_float_address_values.push_back(v->clone());
    }
    
    void int_address_callback(const Value * v)
    {
        m_int_address_values.push_back(v->clone());
    }

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");
        
        local_device->emplace(local_device->children().begin(), "float");
        auto float_address = local_device->children().front()->createAddress(Value::Type::FLOAT);
        
        local_device->emplace(local_device->children().begin(), "int");
        auto int_address = local_device->children().front()->createAddress(Value::Type::INT);

        Float f(0);

        auto mapper = Mapper::create(float_address, int_address, &f);
        QVERIFY(automation != nullptr);

        QVERIFY(automation->getStartState() != nullptr);
        QVERIFY(automation->getEndState() != nullptr);
        QVERIFY(automation->getParentTimeConstraint() == nullptr);

        QVERIFY(automation->getDriverAddress() == float_address);
        QVERIFY(automation->getDrivenAddress() == int_address);
        QVERIFY(*automation->getDriving() == f);

        //! \todo test clone()
    }

    /*! test execution functions */
    //! \todo test state()
    void test_execution()
    {
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");
        
        local_device->emplace(local_device->children().begin(), "float");
        auto float_address = local_device->children().front()->createAddress(Value::Type::FLOAT);
        float_address->addCallback([&] (const Value* v) { float_address_callback(v); });
        
        auto int_address = local_device->children().front()->createAddress(Value::Type::INT);
        int_address->addCallback([&] (const Value* v) { int_address_callback(v); });

        auto curve = Curve<float, int>::create();
        auto linearSegment = CurveSegmentLinear<int>::create(curve);
        curve->setInitialValue(0);
        curve->addPoint(0.5, 5, linearSegment);
        curve->addPoint(1., 10, linearSegment);
        Behavior b(curve);
        auto mapper = Mapper::create(float_address, int_address, &b);

        auto start_node = TimeNode::create();
        auto end_node = TimeNode::create();
        auto event_callback = std::bind(&MapperTest::event_callback, this, _1);
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
        auto constraint_callback = std::bind(&MapperTest::constraint_callback, this, _1, _2, _3);
        auto constraint = TimeConstraint::create(constraint_callback, start_event, end_event, 100.);
        constraint->addTimeProcess(mapper);

        m_float_address_values.clear();
        m_int_address_values.clear();

        constraint->setGranularity(10.);
        constraint->start();

        while (constraint->getRunning())
        {
            std::this_thread::sleep_for( std::chrono::milliseconds(10));
            const Float* current = static_cast<const Float*>(float_address->getValue());
            float_address->pushValue(new Float(current->value + 0.1));
        }

        QVERIFY(m_float_address_values.size() == 9 && m_int_address_values.size() == 9);

        Int zero(0);
        Int dix(10);
        QVERIFY(*m_float_address_values[0] == zero);
        QVERIFY(*m_int_address_values[8] == dix);
    }
};

QTEST_APPLESS_MAIN(MapperTest)

#include "MapperTest.moc"
