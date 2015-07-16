#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class StateTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto state = State::create();
        QVERIFY(state != nullptr);

        QVERIFY(state->stateElements().size() == 0);

        auto substate = State::create();
        state->stateElements().push_back(substate);
        QVERIFY(state->stateElements().size() == 1);

        //! \todo test clone()
    }
    
    /*! test execution functions */
    void test_execution()
    {
        //! \todo test launch()
    }
};

QTEST_APPLESS_MAIN(StateTest)

#include "StateTest.moc"
