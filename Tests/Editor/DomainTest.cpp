#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
class DomainTest : public QObject
{
    Q_OBJECT

        template<typename T>
        void push_all(ossia::net::address_base& addr, T min, T max)
        {
            using val_t = decltype(T{}.value);
            for(int i = 0; i < 6; i++)
            {
                addr.setBoundingMode((bounding_mode)i);
                addr.pushValue(T{(val_t)(min.value - 100)});
                addr.pushValue(min);
                addr.pushValue(T{(val_t)((min.value + max.value) / 2)});
                addr.pushValue(max);
                addr.pushValue(T{(val_t)(max.value + 100)});
            }
        }

        template<typename T>
        void test_clamp_numeric(ossia::net::address_base& addr, T min, T max)
        {
            auto dom = ossia::net::make_domain(min, max);
            addr.setDomain(dom);

            // min and max
            push_all(addr, min, max);

            // no min
            ossia::net::set_min(dom, {});
            push_all(addr, min, max);

            // no max
            ossia::net::set_min(dom, min);
            ossia::net::set_max(dom, {});
            push_all(addr, min, max);

            // nothing
            ossia::net::set_min(dom, {});
            ossia::net::set_max(dom, {});
            push_all(addr, min, max);
        }

        template<typename T>
        void push_tuple(ossia::net::address_base& addr, T min, T max)
        {
            using val_t = decltype(T{}.value);
            // TODO why couldn't domain operate on dataspaces ?
            // e.g. for a position, we could want to limit its norm ?
            // Maybe the domain could be a list of constraint :
            // "default" min max constraint,
            // constraint added by the unit type (e.g. rgb : between 0 / 1)
            // additional constraints..
            Tuple t{Float{(val_t)(min.value - 100)},
                    min,
                    Float{(val_t)((min.value + max.value) / 2)},
                    max,
                    Float{(val_t)(max.value + 100)}};
            for(int i = 0; i < 6; i++)
            {
                addr.setBoundingMode((bounding_mode)i);
                addr.pushValue(t);
            }
        }


        void test_clamp_tuple(ossia::net::address_base& addr)
        {
            const Float min{0};
            const Float max{1};
            domain_base<Tuple> dom{ossia::value{min}, ossia::value{max}};

            addr.setDomain(dom);

            // min and max
            push_all(addr, min, max);

            // no min
            dom.min.reset();
            push_all(addr, min, max);

            // no max
            dom.min = min;
            dom.max.reset();
            push_all(addr, min, max);

            // nothing
            dom.min.reset();
            dom.max.reset();
            push_tuple(addr, min, max);
        }

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

        domain d3 = make_domain(Int(1), Int(24));
        QVERIFY(d3 == domain_base<Int>(1, 24));
        QVERIFY(d3 != dom);
    }

    void test_clamp()
    {
        TestUtils t;
        test_clamp_numeric(*t.float_addr, Float{0.}, Float{1.});
        test_clamp_numeric(*t.int_addr, Int{0}, Int{100});
        test_clamp_numeric(*t.bool_addr, Bool{false}, Bool{true});
        test_clamp_numeric(*t.char_addr, Char{'a'}, Char{'z'});

    }


};

QTEST_APPLESS_MAIN(DomainTest)

#include "DomainTest.moc"
