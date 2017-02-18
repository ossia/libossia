#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;

class AddressTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};
        auto cld = device.createChild("child");
        auto address = cld->createAddress();
        QVERIFY(address != nullptr);
        if(address == nullptr)
          return;

        QVERIFY(&address->getNode() == device.children().front().get());
        QVERIFY(&address->getNode().getDevice() == &device);

        QVERIFY(address->getValueType() == val_type::IMPULSE);

        address->setValueType(val_type::INT);
        QVERIFY(address->getValueType() == val_type::INT);

        QVERIFY(address->getAccessMode() == ossia::access_mode::BI);

        address->setAccessMode(ossia::access_mode::SET);
        QVERIFY(address->getAccessMode() == ossia::access_mode::SET);

        QVERIFY(!address->getDomain());

        address->setDomain(make_domain(0, 100));
        QVERIFY(address->getDomain() == make_domain(0, 100));

        QVERIFY(address->getBoundingMode() == ossia::bounding_mode::FREE);

        address->setBoundingMode(ossia::bounding_mode::CLIP);
        QVERIFY(address->getBoundingMode() == ossia::bounding_mode::CLIP);

        QVERIFY(address->getRepetitionFilter() == repetition_filter::OFF);

        address->setRepetitionFilter(repetition_filter::ON);
        QVERIFY(address->getRepetitionFilter() == repetition_filter::ON);

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

