#include <QtTest>
#include <ossia/ossia.hpp>
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
        ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

        // Local tree building
        auto localIntNode = device.create_child("my_int");
        auto localIntAddress = localIntNode->create_address(val_type::INT);

        message message{*localIntAddress, 10};

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
