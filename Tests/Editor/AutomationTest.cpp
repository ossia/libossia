#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

void automation_callback(const TimeValue&, const TimeValue&, std::shared_ptr<State>)
{
    ;
}

class AutomationTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        Local local_protocol{};
        auto local_device = Device::create(local_protocol, "test");

        local_device->emplace(local_device->children().begin(), "child");
        auto address = local_device->children().front()->createAddress(Value::Type::FLOAT);

        Float f(0);
        auto automation = Automation::create(&automation_callback, address, &f);
        QVERIFY(automation != nullptr);

        QVERIFY(automation->getStartState() != nullptr);
        QVERIFY(automation->getEndState() != nullptr);
        QVERIFY(automation->getClock() != nullptr);
        QVERIFY(automation->getParentTimeConstraint() == nullptr);

        QVERIFY(automation->getDrivenAddress() == address);
        QVERIFY(*automation->getDriving() == f);

        //! \todo test clone()
    }
    
    /*! test execution functions */
    void test_execution()
    {
        //! \todo test play()
        //! \todo test stop()
        //! \todo test pause()
        //! \todo test resume()
        //! \todo test state()
    }
};

QTEST_APPLESS_MAIN(AutomationTest)

#include "AutomationTest.moc"
