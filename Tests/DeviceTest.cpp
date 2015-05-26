#include <QtTest>

#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol.h"

using namespace OSSIA;

class DeviceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createDeviceTest()
    {
        Protocol none;
        auto deviceNoProtocole = Device::create(none, "void protocol");

        Minuit minuit{"127.0.0.1", 7000, 7001};
        auto minDev = Device::create(minuit, "minuitDevice");

        QCOMPARE(minDev->updateNamespace(), false);
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

