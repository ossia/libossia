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

        QCOMPARE(s.size(), 0ul);

        state substate;
        s.add(substate);
        QCOMPARE(s.size(), 1ul);


        state parent;
        parent.add(std::move(s));
        QCOMPARE(parent.size(), 1ul);
        QCOMPARE(s.size(), 0ul);

        state copy{parent};
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
