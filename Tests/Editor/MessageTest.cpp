#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class MessageTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    
    /*! test life cycle and accessors functions */
    void test_basic()
    {
        // Local device
        auto local_protocol = Local::create();
        auto device = Device::create(local_protocol, "test");

        // Local tree building
        auto localIntNode = *(device->emplace(device->children().cend(), "my_int"));
        auto localIntAddress = localIntNode->createAddress(Type::INT);

        auto message = Message::create(localIntAddress, new Int(10));
        QVERIFY(message != nullptr);

        QVERIFY(message->getAddress() == localIntAddress);

        QVERIFY(*message->getValue() == Int(10));

        //! \todo test clone()
    }
    
    /*! test execution functions */
    void test_execution()
    {
        ;
    }
};

QTEST_APPLESS_MAIN(MessageTest)

#include "MessageTest.moc"
