#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class DeviceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        ossia::net::generic_device local_device{
          std::make_unique<ossia::net::local_protocol>(), "test"};
        ossia::net::generic_device osc_device{
          std::make_unique<ossia::net::OSC2>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device minuit_device{
          std::make_unique<ossia::net::minuit_protocol>("127.0.0.1", 13579, 13580), "test_minuit"};
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

