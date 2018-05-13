// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/base/parameter.hpp>
using namespace ossia;

/*
struct mock_autom2
{
  ossia::Destination mDestination;
  ossia::unit_t mUnit;
  ossia::functional_state state()
  {
    ossia::functional_state autom_h;

    // For an autom, same input & output
    autom_h.inputs.push_back(mDestination);
    autom_h.outputs.push_back(mDestination);
    autom_h.func = [=] (std::vector<ossia::value_with_unit>& v)
    {
      assert(v.size() == 1);
      auto& s = v[0];

      // destination_index ? how to say that we change h ? [0] + hsv...

      // 2. potentially convert value to the automation dataspace
      // e.g. if the automation changes h in hsv, we convert s to hsv
      std::cerr << "before conversion: " << to_pretty_string(s) << std::endl;
      if(mUnit)
      {
        s = convert(s, mUnit);
      }
      std::cerr << "after conversion: " << to_pretty_string(s) << std::endl;

      // 3. merge the automation result in s
      s = merge(s, computeValue(), mDestination.index);
      std::cerr << "after merge: " << to_pretty_string(s) << std::endl;


    };

    return autom_h;
  }

  float computeValue() { return 0.8; }
};
  void test_convert()
  {
    auto res = ossia::convert(ossia::centimeter(5),  ossia::millimeter_u{});

    QVERIFY(res == ossia::millimeter{50});
    QVERIFY(res != ossia::millimeter{25});
    QVERIFY(res != ossia::centimeter{5});
    convert(ossia::centimeter(5),  ossia::rgb_u{});

    qDebug() << sizeof(ossia::unit_t)
             << sizeof(ossia::value)
             << sizeof(ossia::value_with_unit)
             << sizeof(ossia::color_u)
             << sizeof(ossia::color)
             << sizeof(rgb_u)
             << sizeof(eggs::variant<rgb_u>);
  }

  void test_functional()
  {
    generic_device dev{std::make_unique<local_protocol>(), "test"};
    auto n1 = dev.create_child("n1")->create_parameter(val_type::LIST);

    ossia::Destination d{*n1};

    // change the V of HSV
    mock_autom2 autom_v{{*n1, {2}}, ossia::hsv_u{}};
    // To change the hue, the whole value is required
    // -> extend to taking the whole value irrelevant of the destination index ?
    // -> who should handle this ?

    mock_autom2 autom_r{{*n1, {0}}, ossia::rgb_u{}};

    functional_state_composition f;
    f.call_chain.push_back(autom_v.state());
    f.call_chain.push_back(autom_r.state());

    ossia::rgb col{std::array<float, 3>{0.3, 0.4, 0.6}};
    std::vector<ossia::value_with_unit> vec; vec.push_back(col);

    std::cerr << "First: " << to_pretty_string(col) << std::endl;
    f.call_chain[0].func(vec);
    std::cerr << "2: " << to_pretty_string(vec[0]) << std::endl;
    f.call_chain[1].func(vec);
    std::cerr << "3: " << to_pretty_string(vec[0]) << std::endl;


    // We have to make a difference between the values that map to the neutral
    // unit and the ones that won't. Impossible.

    // How to handle the case where changing the hue actually changes the whole
    // color, but changing just "r" does not ?


    // Case 2 : mappings

    // Mapping 1 : a:/b[hue] -> c:/d[sat]
    // Mapping 2 : c:/d[r] -> e:/f[r]
  }
  */



using namespace ossia::net;
class StateTest : public QObject
{
  Q_OBJECT

private:
  /*! test life cycle and accessors functions */
  void test_basic()
  {
    state s;

    QCOMPARE((int32_t)s.size(), 0);

    state substate;
    s.add(substate);
    QCOMPARE((int32_t)s.size(), 1);


    state parent;
    parent.add(std::move(s));
    QCOMPARE((int32_t)parent.size(), 1);
    QCOMPARE((int32_t)s.size(), 0);

    state copy{parent};
    QCOMPARE((int32_t)copy.size(), 1);

  }

  void test_compare()
  {
    generic_device dev{std::make_unique<multiplex_protocol>(), "test"};
    auto n1 = dev.create_child("n1")->create_parameter(val_type::LIST);

    message m0{{*n1, ossia::destination_index{0}}, 5.};
    message m1{{*n1, ossia::destination_index{0}}, 5.};
    QVERIFY(m0 == m1);

    state s1, s2;
    QVERIFY(s1 == s2);

    s1.add(m0);
    QVERIFY(s1 != s2);
    QVERIFY(ossia::state_element(m0) != s1);
    QVERIFY(ossia::state_element(m0) != s2);

    s2.add(m1);
    QVERIFY(s1 == s2);

  }

  void test_remove()
  {
    generic_device dev{std::make_unique<multiplex_protocol>(), "test"};
    auto n1 = dev.create_child("n1")->create_parameter(val_type::LIST);

    state s;

    message m0{{*n1, ossia::destination_index{0}}, float{5.}};
    s.add(m0);
    s.remove(m0);
    QCOMPARE((int)s.size(), 0);
  }

  void test_flatten()
  {
    generic_device dev{std::make_unique<multiplex_protocol>(), "test"};
    auto n1 = dev.create_child("n1")->create_parameter(val_type::LIST);

    message m0{{*n1, ossia::destination_index{0}}, float{5.}};
    message m1{{*n1, ossia::destination_index{1}}, float{10.}};
    message m2{{*n1, ossia::destination_index{2}}, float{15.}};

    state s1;
    flatten_and_filter(s1, m0);
    flatten_and_filter(s1, m1);
    flatten_and_filter(s1, m2);

    QCOMPARE((int)s1.size(), 1);

    const piecewise_message* pw = s1.children()[0].target<piecewise_message>();
    QVERIFY(pw);
    QVERIFY(pw->address == m0.dest.value);
    std::vector<ossia::value> expected{float{5.}, float{10.}, float{15.}};
    QVERIFY(pw->message_value == expected);

    // permutations
    state s2;
    flatten_and_filter(s2, m2);
    flatten_and_filter(s2, m1);
    flatten_and_filter(s2, m0);

    state s3;
    flatten_and_filter(s3, m0);
    flatten_and_filter(s3, m2);
    flatten_and_filter(s3, m1);

    QVERIFY(s1 == s2);
    QVERIFY(s1 == s3);
    QVERIFY(s2 == s3);


    flatten_and_filter(s1, m0);
    QVERIFY(s1 == s2);

    // Changing a value does overwrite
    message m0_bis{{*n1, ossia::destination_index{0}}, float{7.}};
    flatten_and_filter(s1, m0_bis);

    ossia::print(std::cerr, s1.children()[0]);
    std::cerr << std::endl;
    state_element expected_bis = piecewise_message{*n1, std::vector<ossia::value>{float{7.}, float{10.}, float{15.}}, {}};
    QVERIFY(s1.children()[0] == expected_bis);
  }

  void test_flatten_move()
  {
    generic_device dev{std::make_unique<multiplex_protocol>(), "test"};
    auto n1 = dev.create_child("n1")->create_parameter(val_type::LIST);

    state_element m0 = message{{*n1, ossia::destination_index{0}}, float{5.}};
    state_element m1 = message{{*n1, ossia::destination_index{1}}, float{10.}};
    state_element m2 = message{{*n1, ossia::destination_index{2}}, float{15.}};

    state s1;
    flatten_and_filter(s1, m0);
    flatten_and_filter(s1, m1);
    flatten_and_filter(s1, m2);

    QCOMPARE((int)s1.size(), 1);

    const piecewise_message* pw = s1.children()[0].target<piecewise_message>();
    QVERIFY(pw);
    QVERIFY(pw->address == m0.target<message>()->dest.value);
    std::vector<ossia::value> expected{float{5.}, float{10.}, float{15.}};
    QVERIFY(pw->message_value == expected);

    // permutations
    state s2;
    flatten_and_filter(s2, m2);
    flatten_and_filter(s2, m1);
    flatten_and_filter(s2, m0);

    state s3;
    flatten_and_filter(s3, m0);
    flatten_and_filter(s3, m2);
    flatten_and_filter(s3, m1);

    QVERIFY(s1 == s2);
    QVERIFY(s1 == s3);
    QVERIFY(s2 == s3);


    flatten_and_filter(s1, std::move(m0));
    QVERIFY(s1 == s2);

    // Changing a value does overwrite
    message m0_bis{{*n1, ossia::destination_index{0}}, float{7.}};
    flatten_and_filter(s1, m0_bis);

    state_element expected_bis = piecewise_message{
            *n1,
            std::vector<ossia::value>{float{7.}, float{10.}, float{15.}}, {}};
    QVERIFY(s1.children()[0] == expected_bis);
  }

  /*! test execution functions */
  void test_execution()
  {
    //! \todo test launch()
  }
};

QTEST_APPLESS_MAIN(StateTest)

#include "StateTest.moc"
