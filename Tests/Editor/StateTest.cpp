#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class StateTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        State state;

        QCOMPARE(state.size(), 0ul);

        State substate;
        state.add(substate);
        QCOMPARE(state.size(), 1ul);


        State parent;
        parent.add(std::move(state));
        QCOMPARE(parent.size(), 1ul);
        QCOMPARE(state.size(), 0ul);

        State copy{parent};
        QCOMPARE(copy.size(), 1ul);

    }

    /*! test execution functions */
    void test_execution()
    {
        //! \todo test launch()
    }
};

QTEST_APPLESS_MAIN(StateTest)

#include "StateTest.moc"
