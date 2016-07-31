#include <QtTest>
#include <ossia/ossia.hpp>
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
        domain_base<Int> dom(1, 24);

        QVERIFY(*dom.min  == 1);
        QVERIFY(*dom.max  == 24);

        dom.values = {1, 10, 24};
        QVERIFY(dom.values.size() == 3);

        auto copy = dom;
        QCOMPARE(copy, dom);

        domain d1 = dom, d2 = copy;
        QCOMPARE(d1, d2);

        d2 = domain_base<Int>{0, 25};
        QVERIFY(d1 != d2);

        d2 = domain_base<Float>{1., 24.};
        QVERIFY(d1 != d2);

        domain d3 = makeDomain(Int(1), Int(24));
        QVERIFY(d3 == domain_base<Int>(1, 24));
        QVERIFY(d3 != dom);
    }
};

QTEST_APPLESS_MAIN(DomainTest)

#include "DomainTest.moc"
