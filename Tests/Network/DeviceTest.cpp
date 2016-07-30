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
        impl::BasicDevice local_device{
          std::make_unique<impl::Local2>(), "test"};
        impl::BasicDevice osc_device{
          std::make_unique<impl::OSC2>("127.0.0.1", 9996, 9997), "test_osc"};
        impl::BasicDevice minuit_device{
          std::make_unique<impl::Minuit2>("127.0.0.1", 13579, 13580), "test_minuit"};
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

