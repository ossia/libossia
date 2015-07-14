#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

class NodeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        Local local_protocol{};
        auto local_device = Device::create(local_protocol, "test");

        local_device->emplace(local_device->children().begin(), "child");
        auto node = local_device->children().front();
        QVERIFY(node != nullptr);
        QVERIFY(local_device->children().size() == 1);

        QVERIFY(node->getParent() == local_device);

        QVERIFY(node->getName() == "child");

        QVERIFY(node->getAddress() == nullptr);
    }

    /*! test edition functions */
    void test_edition()
    {
        Local local_protocol{};
        auto local_device = Device::create(local_protocol, "test");

        local_device->emplace(local_device->children().begin(), "child");
        auto node = local_device->children().front();

        auto address = node->createAddress();
        QVERIFY(address != nullptr);
        QVERIFY(address == node->getAddress());

        QVERIFY(node->removeAddress());
        QVERIFY(node->getAddress() == nullptr);
    }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
