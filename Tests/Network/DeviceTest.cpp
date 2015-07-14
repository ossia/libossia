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
        auto local_device = Device::create(local_protocol, "test");
        QVERIFY(local_device != nullptr);
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

