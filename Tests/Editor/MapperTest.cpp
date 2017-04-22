#include <QtTest>
#include <ossia/ossia.hpp>
#include <functional>
#include <thread>
#include <iostream>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono;
using namespace std::placeholders;

class MapperTest : public QObject
{
  Q_OBJECT

  address_base* m_float_address{};
  std::vector<value> m_float_address_values;

  address_base* m_int_address{};
  std::vector<value> m_int_address_values;

  void constraint_callback(ossia::time_value position, time_value date, const state& st)
  {
    st.launch();
  }

  void event_callback(time_event::status newStatus)
  {
    std::cout << "Event : " << "new status received" << std::endl;
  }

  void int_address_callback(const value& v)
  {
    // store mapping result
    m_int_address_values.push_back(v);

    // store current float value
    m_float_address_values.push_back(m_float_address->value());

    // prepare next float value
    const float current = m_float_address->value().get<float>();
    m_float_address->push_value(current + 0.5);
  }

private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    auto float_n = device.create_child("float");
    ossia::net::address_base* float_address = float_n->create_address(val_type::FLOAT);

    auto int_n = device.create_child("int");
    auto int_address = int_n->create_address(val_type::INT);

    behavior b;

    std::shared_ptr<mapper> mapping = std::make_shared<mapper>(*float_address, *int_address, b);
    QVERIFY(mapping != nullptr);

    QVERIFY(mapping->parent() == nullptr);
    //! \todo test clone()
  }

  /*! test execution functions */
  //! \todo test state()
  void test_execution()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    auto float_n = device.create_child("float");
    m_float_address = float_n->create_address(val_type::FLOAT);

    auto int_n = device.create_child("int");
    m_int_address = int_n->create_address(val_type::INT);
    auto int_address_callback = std::bind(&MapperTest::int_address_callback, this, _1);
    m_int_address->add_callback(int_address_callback);

    auto c = std::make_shared<curve<float, int>>();
    curve_segment_linear<int> linearSegment;
    c->set_x0(-10.);
    c->set_y0(-10);
    c->add_point(linearSegment, 10., 10);

    auto start_node = std::make_shared<time_node>();
    auto end_node = std::make_shared<time_node>();
    auto event_callback = std::bind(&MapperTest::event_callback, this, _1);
    auto start_event = *(start_node->emplace(start_node->get_time_events().begin(), event_callback));
    auto end_event = *(end_node->emplace(end_node->get_time_events().begin(), event_callback));
    auto constraint_callback = std::bind(&MapperTest::constraint_callback, this, _1, _2, _3);
    auto constraint = time_constraint::create(constraint_callback, *start_event, *end_event, 400._tv, 400._tv, 400._tv);
    constraint->add_time_process(
          std::make_unique<mapper>(Destination{*m_float_address}, Destination{*m_int_address}, c));

    m_float_address_values.clear();
    m_int_address_values.clear();

    float f(-10.);
    m_float_address->push_value(f);

    constraint->set_granularity(10._tv);
    constraint->start();

    while (constraint->running())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    QVERIFY(m_float_address_values.size() == m_int_address_values.size());

    // check if each value produced by the mapping is correct
    auto it = m_int_address_values.begin();
    for (auto v : m_float_address_values)
    {
      float f = v.get<float>();
      int i = it->get<int>();

      int result = c->value_at(f);

      QVERIFY(i == result);

      it++;
    }
  }
};

QTEST_APPLESS_MAIN(MapperTest)

#include "MapperTest.moc"
