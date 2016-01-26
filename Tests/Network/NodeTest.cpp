#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;
using namespace std::placeholders;

class NodeTest : public QObject
{
    Q_OBJECT

    std::string node_emplaced;
    std::string node_erased;
    std::string node_renamed;
    std::string node_address_created;
    std::string node_address_removed;

    void node_change_callback(const OSSIA::Node& node, const std::string& name, OSSIA::NodeChange change)
    {
        switch (change)
        {
        case NodeChange::EMPLACED:
            node_emplaced = name;
            break;
        case NodeChange::ERASED:
            node_erased = name;
            break;
        case NodeChange::RENAMED:
            node_renamed = name;
            break;
        case NodeChange::ADDRESS_CREATED:
            node_address_created = name;
            break;
        case NodeChange::ADDRESS_REMOVED:
            node_address_removed = name;
            break;
        }
    }

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
            std::shared_ptr<OSSIA::Node> node = local_device->children().front();

            auto address = node->createAddress();
            QVERIFY(node->getAddress() != nullptr);
            QVERIFY(address == node->getAddress());

            QVERIFY(node->removeAddress());
            QVERIFY(node->getAddress() == nullptr);

            bool removed = false;
            auto& children = local_device->children();
            auto it = std::find_if(children.begin(), children.end(),
                                   [&] (const std::shared_ptr<OSSIA::Node>& elt) { return elt->getName() == "child"; });
            if(it != children.end())
            {
               local_device->erase(it);
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

    /*! test callback notifications */
    void test_callback()
    {
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");
        auto callback = std::bind(&NodeTest::node_change_callback, this, _1, _2, _3);

        CallbackContainer<NodeChangeCallback>::iterator local_device_callback_it = local_device->addCallback(callback);
        QVERIFY(local_device->callbacks().size() == 1);

        // edit a node and its address, rename it and then remove it
        {
            Container<OSSIA::Node>::iterator it = local_device->emplace(local_device->children().begin(), "child");
            std::shared_ptr<OSSIA::Node> node = *it;

            QVERIFY(node_emplaced == "child");

            // the node should be observed by the local device
            QVERIFY(node->callbacks().size() == 1);

            // add a callback on the node itself to get any changes on it
            CallbackContainer<NodeChangeCallback>::iterator node_callback_it = node->addCallback(callback);
            QVERIFY(node->callbacks().size() == 2);

            node->createAddress();
            QVERIFY(node_address_created == "child");

            node->removeAddress();
            QVERIFY(node_address_removed == "child");

            node->setName("foo");
            QVERIFY(node_renamed == "child");

            // don't need to observe the node itself anymore
            node->removeCallback(node_callback_it);
            QVERIFY(node->callbacks().size() == 1);

            local_device->erase(it);
            QVERIFY(node_erased == "foo");
        }

        local_device->removeCallback(local_device_callback_it);
        QVERIFY(local_device->callbacks().size() == 0);
    }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
