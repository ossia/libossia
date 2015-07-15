#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class MessageTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        // Local device
        Local device_parameters{};
        auto device = Device::create(device_parameters, "test");

        // Local tree building
        auto localIntNode = *(device->emplace(device->children().cend(), "my_int"));
        auto localIntAddress = localIntNode->createAddress(Value::Type::INT);

        auto message = Message::create(localIntAddress, new Int(10));
        QVERIFY(message != nullptr);

        QVERIFY(message->getAddress() == localIntAddress);

        QVERIFY(*message->getValue() == Int(10));
    }
    
    /*! test execution functions */
    void test_execution()
    {
        ;
    }
};

QTEST_APPLESS_MAIN(MessageTest)

#include "MessageTest.moc"
