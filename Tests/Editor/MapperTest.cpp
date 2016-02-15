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

    std::shared_ptr<Address> m_float_address;
    std::vector<Value*> m_float_address_values;

    std::shared_ptr<Address> m_int_address;
    std::vector<Value*> m_int_address_values;

    void constraint_callback(const TimeValue& position, const TimeValue& date, std::shared_ptr<StateElement> element)
    {
        element->launch();
    }

    void event_callback(TimeEvent::Status newStatus)
    {
        std::cout << "Event : " << "new status received" << std::endl;
    }

    void int_address_callback(const Value * v)
    {
        // store mapping result
        m_int_address_values.push_back(v->clone());

        // store current float value
        m_float_address_values.push_back(m_float_address->getValue()->clone());

        // prepare next float value
        const Float* current = static_cast<const Float*>(m_float_address->getValue());
        m_float_address->pushValue(new Float(current->value + 0.5));
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
        m_float_address = local_device->children().front()->createAddress(Value::Type::FLOAT);

        local_device->emplace(local_device->children().begin(), "int");
        m_int_address = local_device->children().front()->createAddress(Value::Type::INT);
        auto int_address_callback = std::bind(&MapperTest::int_address_callback, this, _1);
        m_int_address->addCallback(int_address_callback);

        auto curve = Curve<float, int>::create();
        auto linearSegment = CurveSegmentLinear<int>::create(curve);
        curve->setInitialPointAbscissa(-10.);
        curve->setInitialPointOrdinate(-10);
        curve->addPoint(linearSegment, 10., 10);

        Behavior b(curve);
        auto mapper = Mapper::create(m_float_address, m_int_address, &b);

        auto start_node = TimeNode::create();
        auto end_node = TimeNode::create();
        auto event_callback = std::bind(&MapperTest::event_callback, this, _1);
        auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
        auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
        auto constraint_callback = std::bind(&MapperTest::constraint_callback, this, _1, _2, _3);
        auto constraint = TimeConstraint::create(constraint_callback, start_event, end_event, 400., 400., 400.);
        constraint->addTimeProcess(mapper);

        m_float_address_values.clear();
        m_int_address_values.clear();

        Float f(-10.);
        m_float_address->pushValue(&f);

        constraint->setGranularity(10.);
        constraint->start();

        while (constraint->getRunning())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        QVERIFY(m_float_address_values.size() == m_int_address_values.size());

        // check if each value produced by the mapping is correct
        std::vector<Value*>::iterator it = m_int_address_values.begin();

        for (auto v : m_float_address_values)
        {
            Float* f = static_cast<Float*>(v);
            Int* i = static_cast<Int*>(*it);

            int result = curve->valueAt(f->value);

            QVERIFY(i->value == result);

            it++;
        }
    }
};

QTEST_APPLESS_MAIN(MapperTest)

#include "MapperTest.moc"
