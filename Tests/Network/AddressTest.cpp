#include <QtTest>

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol.h"

#include <iostream>

using namespace OSSIA;

class AddressTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void addressMainTest()
    {
        Local loc{};
        auto localDev = Device::create(loc, "localDev");
        localDev->emplace(localDev->children().begin(), "child");

        auto node = localDev->children().front();
        auto addr = node->createAddress(AddressValue::Type::INT);

        Int i(1);
        QCOMPARE(addr->sendValue(&i), true);
        QCOMPARE(addr->getValueType(), AddressValue::Type::INT);

        QCOMPARE(addr->updateValue(), true);
        QCOMPARE((static_cast<Int*>(addr->getValue()))->value, 1);
        QCOMPARE((static_cast<Int*>(addr->getValue()))->value, 1);
    }
};

QTEST_APPLESS_MAIN(AddressTest)

#include "AddressTest.moc"

