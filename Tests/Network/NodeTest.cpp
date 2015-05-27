#include <QtTest>

#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol.h"

#include "Network/AddressValue.h"

using namespace OSSIA;

class NodeTest :public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void basicnodetest()
    {
        Local local{};
        auto localDevice = Device::create(local, "localProtocol");

        Minuit minuit{"127.0.0.1", 7000, 7001};
        auto minuitDev = Device::create(minuit, "minuit_device");

        std::string nodeName("fils");
        minuitDev->emplace(minuitDev->children().begin(), nodeName);
        QCOMPARE(int(minuitDev->children().size()), 1 );

        auto node = minuitDev->children().front();
        QCOMPARE(node->getName(), nodeName);

        QCOMPARE(node->getParent().getName(), minuitDev->getName());

        OSC osc{"127.0.0.1", 9998, 9999};
        auto oscDevice = Device::create(osc, "osc_device");


        auto address = node->createAddress(AddressValue::Type::BOOL);
        QVERIFY(address != nullptr );
        QCOMPARE(node->getAddress(), address);

        QCOMPARE(node->removeAddress(), true);

    }

    void childNodetest()
    {
        Local local{};
        auto localDevice = Device::create(local, "localProtocol");

        std::string nodeName("parent");
        localDevice->emplace(localDevice->children().begin(), nodeName);
        QCOMPARE(int(localDevice->children().size()), 1 );

        auto parentNode = localDevice->children().front();

        parentNode->emplace(parentNode->children().begin(), "fils1");
        parentNode->emplace(parentNode->children().begin(), "fils2");
        parentNode->emplace(parentNode->children().begin(), "fils3");
        //TODO ordre de rangement de emplace

        auto first_child = parentNode->children().begin();

        //QCOMPARE(first_child->getName(), std::string("fils3"));

    }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"

