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

    behavior b;

    auto autom = std::make_shared<automation>(*address, b);
    QVERIFY(autom != nullptr);

    QVERIFY(autom->parent() == nullptr);

    QVERIFY(autom->getDrivenAddress() == *address);
    QVERIFY(autom->getDriving() == b);

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
    c->setInitialPointOrdinate(0.);
    c->addPoint(linearSegment, 0.5, 1.);
    c->addPoint(linearSegment, 1., 0.);


    auto start_node = std::make_shared<time_node>();
    auto end_node = std::make_shared<time_node>();
    auto event_callback = std::bind(&AutomationTest::event_callback, this, _1);
    auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
    auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
    auto constraint_callback = std::bind(&AutomationTest::constraint_callback, this, _1, _2, _3);
    auto constraint = time_constraint::create(constraint_callback, *start_event, *end_event, 100._tv, 100._tv, 100._tv);
    constraint->addTimeProcess(std::make_unique<automation>(*address, c));

    m_address_values.clear();

    constraint->setGranularity(10._tv);
    constraint->start();

    while (constraint->getRunning())
      ;
    // Let the time for callbacks to happen...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    QVERIFY(m_address_values.size() == 10 || m_address_values.size() == 11 || m_address_values.size() == 12);

    Float zero(0);
    QVERIFY(m_address_values.front() == zero);
    QVERIFY(m_address_values.back() == zero);

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
    m_address_values.clear();
  }

  void test_dataspace()
  {

    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};
    auto cld = device.createChild("child");
    auto address = cld->createAddress(val_type::VEC3F);
    address->setUnit(ossia::rgb_u{});
    address->pushValue(make_vec(0.1, 0.1, 0.1));

    address->add_callback([&] (const value&v) { address_callback(v); });

    auto c = std::make_shared<curve<double, float>>();
    curve_segment_linear<float> linearSegment;

    c->setInitialPointAbscissa(0.);
    c->setInitialPointOrdinate(0.);
    c->addPoint(linearSegment, 1., 1.);

    // TODO make a "make_base_scenario" function.
    auto start_node = std::make_shared<time_node>();
    auto end_node = std::make_shared<time_node>();
    auto event_callback = std::bind(&AutomationTest::event_callback, this, _1);
    auto start_event = *(start_node->emplace(start_node->timeEvents().begin(), event_callback));
    auto end_event = *(end_node->emplace(end_node->timeEvents().begin(), event_callback));
    auto constraint_callback = std::bind(&AutomationTest::constraint_callback, this, _1, _2, _3);
    auto constraint = time_constraint::create(constraint_callback, *start_event, *end_event, 100._tv, 100._tv, 100._tv);

    auto autom = std::make_unique<automation>(Destination{*address, {2}, ossia::hsv_u{}}, c);

    auto& tp = (ossia::time_process&) *autom;

    constraint->addTimeProcess(std::move(autom));

    auto state = tp.offset(constraint->getDurationNominal() * 0.5);
    auto mess = state.target<ossia::message>() ;
    QVERIFY(mess != nullptr);

    ossia::message expected{Destination{*address, {2}, ossia::hsv_u{}}, Float{0.5}};

    ossia::print(std::cerr, *mess);
    ossia::print(std::cerr, expected);
    QCOMPARE(*mess, expected);


  }
};

QTEST_APPLESS_MAIN(AutomationTest)

#include "AutomationTest.moc"
