// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/network/phidgets/phidgets_protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::net;
class PhidgetTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_exec()
    {
      auto phid = new ossia::phidget_protocol;
      auto prot = new ossia::net::multiplex_protocol;
      ossia::net::generic_device dev{std::unique_ptr<ossia::net::multiplex_protocol>(prot), "phidgets"};
      prot->expose_to(std::unique_ptr<ossia::phidget_protocol>(phid));
      prot->expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>());


      sleep(2);
      phid->run_commands();

      sleep(10);
    }
};


QTEST_APPLESS_MAIN(PhidgetTest)

#include "PhidgetTest.moc"

