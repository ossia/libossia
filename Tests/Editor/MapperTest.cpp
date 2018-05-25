//// This is an open source non-commercial project. Dear PVS-Studio, please check it.
//// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
//#include <ossia/ossia.hpp>
//#include <functional>
//#include <thread>
//#include <iostream>

//using namespace ossia;
//using namespace ossia::net;
//using namespace std::chrono;
//using namespace std::placeholders;

class MapperTest : public QObject
{
  Q_OBJECT

//  parameter_base* m_float_address{};
//  std::vector<value> m_float_parameter_values;

//  parameter_base* m_int_address{};
//  std::vector<value> m_int_parameter_values;

//  void interval_callback(double position, ossia::time_value date)
//  {
//  }

//  void event_callback(time_event::status newStatus)
//  {
//    std::cout << "Event : " << "new status received" << std::endl;
//  }

//  void int_parameter_callback(const value& v)
//  {
//    // store mapping result
//    m_int_parameter_values.push_back(v);

//    // store current float value
//    m_float_parameter_values.push_back(m_float_address->value());

//    // prepare next float value
//    const float current = m_float_address->value().get<float>();
//    m_float_address->push_value(current + 0.5);
//  }

//private Q_SLOTS:

//  /*! test life cycle and accessors functions */
//  void test_basic()
//  {
//    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

//    auto float_n = device.create_child("float");
//    ossia::net::parameter_base* float_address = float_n->create_parameter(val_type::FLOAT);

//    auto int_n = device.create_child("int");
//    auto int_address = int_n->create_parameter(val_type::INT);

//    behavior b;

//    std::shared_ptr<mapper> mapping = std::make_shared<mapper>(*float_address, *int_address, b);
//    QVERIFY(mapping != nullptr);

//    //! \todo test clone()
//  }

//  /*! test execution functions */
//  //! \todo test state()
//  void test_execution()
//  {
//    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

//    auto float_n = device.create_child("float");
//    m_float_address = float_n->create_parameter(val_type::FLOAT);

//    auto int_n = device.create_child("int");
//    m_int_address = int_n->create_parameter(val_type::INT);
//    auto int_parameter_callback = std::bind(&MapperTest::int_parameter_callback, this, _1);
//    m_int_address->add_callback(int_parameter_callback);

//    auto c = std::make_shared<curve<float, int>>();
//    curve_segment_linear<int> linearSegment;
//    c->set_x0(-10.);
//    c->set_y0(-10);
//    c->add_point(linearSegment, 10., 10);

//    auto start_node = std::make_shared<time_sync>();
//    auto end_node = std::make_shared<time_sync>();
//    auto event_callback = std::bind(&MapperTest::event_callback, this, _1);
//    auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
//    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), event_callback));
//    auto interval_callback = std::bind(&MapperTest::interval_callback, this, _1, _2);
//    auto interval = time_interval::create(interval_callback, *start_event, *end_event, 400._tv, 400._tv, 400._tv);
//    interval->add_time_process(
//          std::make_unique<mapper>(destination{*m_float_address}, destination{*m_int_address}, curve_ptr{c}));
//    ossia::clock clck{*interval};
//    m_float_parameter_values.clear();
//    m_int_parameter_values.clear();

//    float f(-10.);
//    m_float_address->push_value(f);

//    using namespace std::literals;
//    clck.set_granularity(10ms);
//    clck.start_and_tick();

//    while (clck.running())
//    {
//      std::this_thread::sleep_for(std::chrono::milliseconds(5));
//    }

//    QVERIFY(m_float_parameter_values.size() == m_int_parameter_values.size());

//    // check if each value produced by the mapping is correct
//    auto it = m_int_parameter_values.begin();
//    for (auto v : m_float_parameter_values)
//    {
//      float f = v.get<float>();
//      int i = it->get<int>();

//      int result = c->value_at(f);

//      QVERIFY(i == result);

//      it++;
//    }
//  }
};

QTEST_APPLESS_MAIN(MapperTest)

#include "MapperTest.moc"
