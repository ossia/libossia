// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/detail/dataspace_parse.hpp>

#include <iostream>
#include <brigand/algorithms/for_each.hpp>

using namespace ossia;

class AddressTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
        auto cld = device.create_child("child");
        auto address = cld->create_address();
        QVERIFY(address != nullptr);
        if(address == nullptr)
          return;

        QVERIFY(&address->get_node() == device.children().front().get());
        QVERIFY(&address->get_node().get_device() == &device);

        QVERIFY(address->get_value_type() == val_type::IMPULSE);

        address->set_value_type(val_type::INT);
        QVERIFY(address->get_value_type() == val_type::INT);

        QVERIFY(address->get_access() == ossia::access_mode::BI);

        address->set_access(ossia::access_mode::SET);
        QVERIFY(address->get_access() == ossia::access_mode::SET);

        QVERIFY(!address->get_domain());

        address->set_domain(make_domain(0, 100));
        QVERIFY(address->get_domain() == make_domain(0, 100));

        QVERIFY(address->get_bounding() == ossia::bounding_mode::FREE);

        address->set_bounding(ossia::bounding_mode::CLIP);
        QVERIFY(address->get_bounding() == ossia::bounding_mode::CLIP);

        QVERIFY(address->get_repetition_filter() == repetition_filter::OFF);

        address->set_repetition_filter(repetition_filter::ON);
        QVERIFY(address->get_repetition_filter() == repetition_filter::ON);
    }

    void test_units()
    {
      brigand::for_each<dataspace_u_list>([] (auto d_t) {
        using type = typename decltype(d_t)::type;
        brigand::for_each<type>([] (auto u_t) {
          using utype = typename decltype(u_t)::type;
          std::cerr << ossia::get_pretty_unit_text(utype{}) << std::endl;
        });
      });
    }
};

QTEST_APPLESS_MAIN(AddressTest)

#include "AddressTest.moc"

