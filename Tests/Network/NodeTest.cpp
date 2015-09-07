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
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");

        QVERIFY(local_device->getName() == "test");

        local_device->setName("app");
        QVERIFY(local_device->getName() == "app");

        local_device->emplace(local_device->children().begin(), "child");
        auto node = local_device->children().front();
        QVERIFY(node != nullptr);
        QVERIFY(local_device->children().size() == 1);

        QVERIFY(node->getParent() == local_device);

        QVERIFY(node->getName() == "child");

        node->setName("foo");
        QVERIFY(node->getName() == "foo");

        QVERIFY(node->getAddress() == nullptr);

        local_device->emplace(local_device->children().begin(), "foo");
        auto brother = local_device->children().front();

        QVERIFY(brother->getName() == "foo.1");
    }

    /*! test edition functions */
    void test_edition()
    {
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");

        // edit a node and its address and then remove it
        {
            local_device->emplace(local_device->children().begin(), "child");
            auto node = local_device->children().front();

            auto address = node->createAddress();
            QVERIFY(address != nullptr);
            QVERIFY(address == node->getAddress());

            QVERIFY(node->removeAddress());
            QVERIFY(node->getAddress() == nullptr);

            bool removed = false;
            auto& children = local_device->children();
            auto it = std::find_if(children.begin(), children.end(),
                                   [&] (const std::shared_ptr<OSSIA::Node>& elt) { return elt->getName() == "child"; });
            if(it != children.end())
            {
               children.erase(it);
               removed = true;
            }

            QVERIFY(removed);
        }

        // edit the same node again to see if it have been
        // correctly destroyed and removed from the namespace
        {
            local_device->emplace(local_device->children().begin(), "child");
            auto node = local_device->children().front();

            QVERIFY(node->getName() == "child");
        }
    }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
