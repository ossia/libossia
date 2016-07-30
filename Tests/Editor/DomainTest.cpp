#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class DomainTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto domain = Domain::create();
        QVERIFY(domain != nullptr);
        QVERIFY(*domain->getMin() == Impulse());
        QVERIFY(*domain->getMax() == Impulse());
        QVERIFY(domain->getValues().size() == 0);

        domain->setMin(new Int(1));
        QVERIFY(*domain->getMin() == Int(1));

        domain->setMax(new Int(24));
        QVERIFY(*domain->getMax() == Int(24));

        std::vector<const Value*> values = {new Int(1), new Int(10), new Int(24)};
        domain->setValues(values);
        QVERIFY(domain->getValues().size() == 3);

        //! \todo test clone()
    }
};

QTEST_APPLESS_MAIN(DomainTest)

#include "DomainTest.moc"
