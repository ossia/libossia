//// This is an open source non-commercial project. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
//#include <ossia/ossia.hpp>
//#include <functional>
//#include <iostream>

//using namespace ossia;
//using namespace std::placeholders;

class AutomationTest : public QObject
{
  Q_OBJECT

//  std::vector<value> m_parameter_values;

//  void interval_callback(double position, ossia::time_value date)
//  {
//    std::cerr << position << std::endl;
//  }

//  void event_callback(time_event::status newStatus)
//  {
//    std::cout << "Event : " << "new status received" << std::endl;
//  }

//  void address_callback(const value& v)
//  {
//    m_parameter_values.push_back(v);
//  }

//private Q_SLOTS:

//  /*! test life cycle and accessors functions */
//  void test_basic()
//  {
//    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
//    auto cld = device.create_child("child");
//    auto address = cld->create_parameter(val_type::FLOAT);

//    behavior b;

//    auto autom = std::make_shared<automation>(*address, b);
//    QVERIFY(autom != nullptr);

//    //! \todo test clone()
//  }

//  /*! test execution functions */
//  //! \todo test state()
//  void test_execution()
//  {
//    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
//    auto cld = device.create_child("child");
//    auto address = cld->create_parameter(val_type::FLOAT);
//    address->add_callback([&] (const value&v) { address_callback(v); });

//    auto c = std::make_shared<curve<double, float>>();
//    curve_segment_linear<float> linearSegment;

//    c->set_x0(0.);
//    c->set_y0(0.);
//    c->add_point(linearSegment, 0.5, 1.);
//    c->add_point(linearSegment, 1., 0.);


//    auto start_node = std::make_shared<time_sync>();
//    auto end_node = std::make_shared<time_sync>();
//    auto event_callback = std::bind(&AutomationTest::event_callback, this, _1);
//    auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
//    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), event_callback));
//    auto interval_callback = std::bind(&AutomationTest::interval_callback, this, _1, _2);
//    auto interval = time_interval::create(interval_callback, *start_event, *end_event, 100._tv, 100._tv, 100._tv);
//    interval->add_time_process(std::make_unique<automation>(*address, curve_ptr{c}));

//    ossia::clock clck{*interval};
//    m_parameter_values.clear();

//    using namespace std::literals;

//    clck.set_granularity(10ms);
//    clck.start_and_tick();

//    while (clck.running())
//      ;
//    // Let the time for callbacks to happen...
//    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

//    qDebug() << "Received: " << m_parameter_values.size() << "values";

//    float zero(0);
//    QVERIFY(m_parameter_values.front() == zero);
//    QVERIFY(m_parameter_values.back() == zero);

//    // check if all values are differents from the previous value
//    float previous(-1);
//    bool different_from_previous = true;
//    for (auto v : m_parameter_values)
//    {
//      qDebug() << ossia::convert<float>(v) << previous;
//      different_from_previous = (v != previous);
//      if (!different_from_previous)
//        break;
//      previous = v.get<float>();
//    }
//    QVERIFY(different_from_previous);
//    m_parameter_values.clear();
//  }

//  void test_dataspace()
//  {

//    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
//    auto cld = device.create_child("child");
//    auto address = cld->create_parameter(val_type::VEC3F);
//    address->set_unit(ossia::rgb_u{});
//    address->push_value(make_vec(0.1, 0.1, 0.1));

//    address->add_callback([&] (const value&v) { address_callback(v); });

//    auto c = std::make_shared<curve<double, float>>();
//    curve_segment_linear<float> linearSegment;

//    c->set_x0(0.);
//    c->set_y0(0.);
//    c->add_point(linearSegment, 1., 1.);

//    // TODO make a "make_base_scenario" function.
//    auto start_node = std::make_shared<time_sync>();
//    auto end_node = std::make_shared<time_sync>();
//    auto event_callback = std::bind(&AutomationTest::event_callback, this, _1);
//    auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
//    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), event_callback));
//    auto interval_callback = std::bind(&AutomationTest::interval_callback, this, _1, _2);
//    auto interval = time_interval::create(interval_callback, *start_event, *end_event, 100._tv, 100._tv, 100._tv);

//    auto autom = std::make_unique<automation>(destination{*address, {2}, ossia::hsv_u{}}, c);

//    auto& tp = (ossia::time_process&) *autom;

//    interval->add_time_process(std::move(autom));

//    auto state = tp.offset(interval->get_nominal_duration() * 0.5, 0.5);
//    auto mess = state.target<ossia::message>() ;
//    QVERIFY(mess != nullptr);

//    ossia::message expected{destination{*address, {2}, ossia::hsv_u{}}, float{0.5}};

//    ossia::print(std::cerr, *mess);
//    ossia::print(std::cerr, expected);
//    QCOMPARE(*mess, expected);


//  }
};

QTEST_APPLESS_MAIN(AutomationTest)

#include "AutomationTest.moc"
