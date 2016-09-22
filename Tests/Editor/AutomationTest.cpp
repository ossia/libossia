#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <iostream>

using namespace ossia;
using namespace std::placeholders;

class AutomationTest : public QObject
{
  Q_OBJECT

  std::vector<value> m_address_values;

  void constraint_callback(time_value position, time_value date, const state& element)
  {
    element.launch();
  }

  void event_callback(time_event::Status newStatus)
  {
    std::cout << "Event : " << "new status received" << std::endl;
  }

  void address_callback(const value& v)
  {
    m_address_values.push_back(v);
  }

private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};
    auto cld = device.createChild("child");
    auto address = cld->createAddress(val_type::FLOAT);

    Float f(0);

    auto autom = std::make_shared<automation>(*address, f);
    QVERIFY(autom != nullptr);

    QVERIFY(autom->parent() == nullptr);

    QVERIFY(autom->getDrivenAddress() == *address);
    QVERIFY(autom->getDriving() == f);

    //! \todo test clone()
  }

  /*! test execution functions */
  //! \todo test state()
  void test_execution()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};
    auto cld = device.createChild("child");
    auto address = cld->createAddress(val_type::FLOAT);
    address->add_callback([&] (const value&v) { address_callback(v); });

    auto c = std::make_shared<curve<double, float>>();
    curve_segment_linear<float> linearSegment;

    c->setInitialPointAbscissa(0.);
    return;
    c->setInitialPointOrdinate(0.);
    c->addPoint(linearSegment, 0.5, 1.);
    c->addPoint(linearSegment, 1., 0.);


    auto start_node = std::make_shared<time_node>();
    auto end_node = std::make_shared<time_node>();
    auto event_callback = std::bind(&AutomationTest::event_callback, this, _1);
    auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
    auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
    auto constraint_callback = std::bind(&AutomationTest::constraint_callback, this, _1, _2, _3);
    auto constraint = time_constraint::create(constraint_callback, *start_event, *end_event, 100., 100., 100.);
    constraint->addTimeProcess(std::make_unique<automation>(*address, Behavior(c)));

    m_address_values.clear();

    constraint->setGranularity(10.);
    constraint->start();

    while (constraint->getRunning())
      ;

    QVERIFY(m_address_values.size() == 11);

    Float zero(0);
    QVERIFY(m_address_values[0] == zero);
    QVERIFY(m_address_values[10] == zero);

    // check if all values are differents from the previous value
    Float previous(-1);
    bool different_from_previous = true;
    for (auto v : m_address_values)
    {
      different_from_previous = (v != previous);
      if (!different_from_previous)
        break;
      previous = v.get<Float>();
    }
    QVERIFY(different_from_previous);
  }
};

QTEST_APPLESS_MAIN(AutomationTest)

#include "AutomationTest.moc"
