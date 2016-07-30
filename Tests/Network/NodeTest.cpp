#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace ossia::net;
using namespace std::placeholders;

class NodeTest : public QObject
{
  Q_OBJECT
private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};
    QVERIFY(device.getName() == "test");

    device.setName("app");
    QVERIFY(device.getName() == "app");

    device.createChild("child");
    auto& node = device.children().front();
    QVERIFY(node != nullptr);
    QVERIFY(device.children().size() == 1);

    QVERIFY(node->getParent() == &device);

    QVERIFY(node->getName() == "child");

    node->setName("foo");
    QVERIFY(node->getName() == "foo");

    QVERIFY(node->getAddress() == nullptr);

    auto brother = device.createChild("foo");

    QVERIFY(brother->getName() == "foo.1");
  }

  /*! test edition functions */
  void test_edition()
  {
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

    // edit a node and its address and then remove it
    {
      auto node = device.createChild("child");

      auto address = node->createAddress();
      QVERIFY(node->getAddress() != nullptr);
      QVERIFY(address == node->getAddress());

      QVERIFY(node->removeAddress());
      QVERIFY(node->getAddress() == nullptr);

      device.removeChild("child");
      QVERIFY(device.children().size() == 0);
    }

    // edit the same node again to see if it have been
    // correctly destroyed and removed from the namespace
    {
      auto node = device.createChild("child");

      QVERIFY(node->getName() == "child");
    }
  }

  /*! test callback notifications */
  void test_callback()
  {
    impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};

    /* TODO */
  }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
