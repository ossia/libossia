#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class DeviceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        Local local_protocol{};
        auto local_device = Device::create(local_protocol, "test_local");
        QVERIFY(local_device != nullptr);

        OSC osc_protocol{"127.0.0.1", 9996, 9997};
        auto osc_device = Device::create(osc_protocol, "test_osc");
        QVERIFY(osc_device != nullptr);

        Minuit minuit_protocol{"127.0.0.1", 13579, 13580};
        auto minuit_device = Device::create(minuit_protocol, "test_minuit");
        QVERIFY(minuit_device != nullptr);
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

