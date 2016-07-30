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
        impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

        // Local tree building
        auto localIntNode = device.createChild("my_int");
        auto localIntAddress = localIntNode->createAddress(Type::INT);

        Message message{*localIntAddress, Int(10)};

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
