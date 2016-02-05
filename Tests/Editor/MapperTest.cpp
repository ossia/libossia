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
        QVERIFY(mapper != nullptr);

        QVERIFY(mapper->getStartState() != nullptr);
        QVERIFY(mapper->getEndState() != nullptr);
        QVERIFY(mapper->getParentTimeConstraint() == nullptr);

        QVERIFY(mapper->getDriverAddress() == float_address);
        QVERIFY(mapper->getDrivenAddress() == int_address);
        QVERIFY(*mapper->getDriving() == f);

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
        curve->setInitialPointAbscissa(-10.);
        curve->setInitialPointOrdinate(10);
        curve->addPoint(linearSegment, 0., 0);
        curve->addPoint(linearSegment, 10., -5);

        Behavior b(curve);
        auto mapper = Mapper::create(float_address, int_address, &b);

        auto start_node = TimeNode::create();
        auto end_node = TimeNode::create();
        auto event_callback = std::bind(&MapperTest::event_callback, this, _1);
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
        auto constraint_callback = std::bind(&MapperTest::constraint_callback, this, _1, _2, _3);
        auto constraint = TimeConstraint::create(constraint_callback, start_event, end_event, 100., 100., 100.);
        constraint->addTimeProcess(mapper);

        m_float_address_values.clear();
        m_int_address_values.clear();

        constraint->setGranularity(5.);
        constraint->start();

        while (constraint->getRunning())
        {
            std::this_thread::sleep_for( std::chrono::milliseconds(10));
            const Float* current = static_cast<const Float*>(float_address->getValue());
            float_address->pushValue(new Float(current->value + 0.5));
        }

        std::cout << m_float_address_values.size() << endl;

        QVERIFY(m_float_address_values.size() == 9 && m_int_address_values.size() == 9);

        std::vector<Int> expected_result = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5};

        QVERIFY(m_float_address_values.size() == expected_result.size());

        std::vector<Int>::iterator it = expected_result.begin();
        for (auto v : m_float_address_values)
        {
            QVERIFY(*v == *it);
            it++;
        }
    }
};

QTEST_APPLESS_MAIN(MapperTest)

#include "MapperTest.moc"
