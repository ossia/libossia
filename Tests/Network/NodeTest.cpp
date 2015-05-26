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

        std::string nodeName("fils");
        localDevice->emplace(localDevice->front(), nodeName);
        QCOMPARE(int(localDevice->size()), 1 );

        auto node = localDevice->front();
        QCOMPARE(node->getName(), nodeName);

        QCOMPARE(node->getParent().getName(), localDevice->getName());

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
        localDevice->emplace(localDevice->front(), nodeName);
        QCOMPARE(int(localDevice->size()), 1 );

        auto parentNode = localDevice->front();

        parentNode->emplace(parentNode->front(), "fils1");
        parentNode->emplace(parentNode->front(), "fils2");
        parentNode->emplace(parentNode->front(), "fils3");
        //TODO ordre de rangement de emplace

        auto first_child = parentNode->front();

        //QCOMPARE(first_child->getName(), std::string("fils3"));

    }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"

