#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

using namespace ossia::net;
class StateTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

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
      generic_device dev{std::make_unique<local_protocol>(), "test"};
      auto n1 = dev.createChild("n1")->createAddress(val_type::TUPLE);

      message m0{{*n1, {0}}, Float{5.}};
      message m1{{*n1, {0}}, Float{5.}};
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
      generic_device dev{std::make_unique<local_protocol>(), "test"};
      auto n1 = dev.createChild("n1")->createAddress(val_type::TUPLE);

      state s;

      message m0{{*n1, {0}}, Float{5.}};
      s.add(m0);
      s.remove(m0);
      QCOMPARE((int)s.size(), 0);
    }

    void test_flatten()
    {
      generic_device dev{std::make_unique<local_protocol>(), "test"};
      auto n1 = dev.createChild("n1")->createAddress(val_type::TUPLE);

      message m0{{*n1, {0}}, Float{5.}};
      message m1{{*n1, {1}}, Float{10.}};
      message m2{{*n1, {2}}, Float{15.}};

      state s1;
      flatten_and_filter(s1, m0);
      flatten_and_filter(s1, m1);
      flatten_and_filter(s1, m2);

      QCOMPARE((int)s1.size(), 1);

      auto pw = s1.children()[0].target<piecewise_message>();
      QVERIFY(pw);
      QVERIFY(pw->address == m0.destination.value);
      Tuple expected{Float{5.}, Float{10.}, Float{15.}};
      QVERIFY(pw->value == expected);

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
      message m0_bis{{*n1, {0}}, Float{7.}};
      flatten_and_filter(s1, m0_bis);

      state_element expected_bis = piecewise_message{*n1, Tuple{Float{7.}, Float{10.}, Float{15.}}};
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
