// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/detail/dataspace_parse.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>

#include <iostream>
#include <brigand/algorithms/for_each.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include "../Benchmarks/Random.hpp"

using namespace ossia;

class AddressTest : public QObject
{
    Q_OBJECT

    /*! test life cycle and accessors functions */
    Q_SLOT void test_basic()
    {
        ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
        auto cld = device.create_child("child");
        auto address = cld->create_parameter();
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

    void test_try_setup_parameter()
    {
      Random r;
      ossia::net::generic_device device{std::make_unique<ossia::oscquery::oscquery_server_protocol>(6677, 8899), "test"};
      using namespace std::chrono;
      for(int i = 0; i < 5000; i++)
      {
        auto& node = ossia::net::create_node(device.get_root_node(), "/blah");
        auto t0 = steady_clock::now();
        ossia::try_setup_parameter(r.getRandomString(), node);
        auto t1 = steady_clock::now();
        std::cout << duration_cast<std::chrono::nanoseconds>(t1 - t0).count() << std::endl;
      }

    }
};

QTEST_APPLESS_MAIN(AddressTest)

#include "AddressTest.moc"

