#include <QtTest>
#include <ossia/network/phidgets/phidgets_device.hpp>
#include <ossia/network/phidgets/phidgets_protocol.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::net;
class PhidgetTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_exec()
    {
      ossia::phidget_device dev{"phidgets"};
      sleep(1);
      dev.get_protocol().run_commands();
      sleep(1);
    }
};


QTEST_APPLESS_MAIN(PhidgetTest)

#include "PhidgetTest.moc"

