#include <QtTest>
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;

class AddressTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        impl::BasicDevice device{std::make_unique<impl::Local2>(), "test"};
        auto cld = device.createChild("child");
        auto address = cld->createAddress();
        QVERIFY(address != nullptr);

        QVERIFY(&address->getNode() == device.children().front().get());
        QVERIFY(&address->getNode().getDevice() == &device);

        QVERIFY(address->getValueType() == Type::IMPULSE);

        address->setValueType(Type::INT);
        QVERIFY(address->getValueType() == Type::INT);

        QVERIFY(address->getAccessMode() == ossia::AccessMode::BI);

        address->setAccessMode(ossia::AccessMode::SET);
        QVERIFY(address->getAccessMode() == ossia::AccessMode::SET);

        QVERIFY(!address->getDomain());

        address->setDomain(net::makeDomain(Int(0), Int(100)));
        QVERIFY(address->getDomain() == net::makeDomain(Int(0), Int(100)));

        QVERIFY(address->getBoundingMode() == ossia::BoundingMode::FREE);

        address->setBoundingMode(ossia::BoundingMode::CLIP);
        QVERIFY(address->getBoundingMode() == ossia::BoundingMode::CLIP);

        QVERIFY(address->getRepetitionFilter() == RepetitionFilter::OFF);

        address->setRepetitionFilter(true);
        QVERIFY(address->getRepetitionFilter() == RepetitionFilter::ON);

        //! \todo verify addCallback

        //! \todo verify removeCallback

        //! \todo verify getValue

        //! \todo verify cloneValue

        //! \todo verify setValue

        //! \todo verify pushValue

        //! \todo verify pullValue
    }
};

QTEST_APPLESS_MAIN(AddressTest)

#include "AddressTest.moc"

