// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/editor/state/message.hpp>
#include <QtTest>
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
        ossia::net::generic_device device{"test"};

        // Local tree building
        auto localIntNode = device.create_child("my_int");
        auto localIntAddress = localIntNode->create_parameter(val_type::INT);

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
