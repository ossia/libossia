#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::net;
class DomainTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        DomainBase<Int> domain(1, 24);

        QVERIFY(*domain.min  == 1);
        QVERIFY(*domain.max  == 24);

        domain.values = {1, 10, 24};
        QVERIFY(domain.values.size() == 3);

        auto copy = domain;
        QCOMPARE(copy, domain);

        Domain d1 = domain, d2 = copy;
        QCOMPARE(d1, d2);

        d2 = DomainBase<Int>{0, 25};
        QVERIFY(d1 != d2);

        d2 = DomainBase<Float>{1., 24.};
        QVERIFY(d1 != d2);

        Domain d3 = makeDomain(Int(1), Int(24));
        QVERIFY(d3 == DomainBase<Int>(1, 24));
        QVERIFY(d3 != domain);
    }
};

QTEST_APPLESS_MAIN(DomainTest)

#include "DomainTest.moc"
