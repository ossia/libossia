#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

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

    /*! test execution functions */
    void test_execution()
    {
        //! \todo test launch()
    }
};

QTEST_APPLESS_MAIN(StateTest)

#include "StateTest.moc"
