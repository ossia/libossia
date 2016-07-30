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
        auto local_protocol = Local::create();
        auto local_device = Device::create(local_protocol, "test");
        
        local_device->emplace(local_device->children().begin(), "child");
        auto address = local_device->children().front()->createAddress();
        QVERIFY(address != nullptr);
        
        QVERIFY(address->getNode() == local_device->children().front());
        QVERIFY(address->getNode()->getDevice() == local_device);
        
        QVERIFY(address->getValueType() == Type::IMPULSE);
        
        address->setValueType(Type::INT);
        QVERIFY(address->getValueType() == Type::INT);
        
        QVERIFY(address->getAccessMode() == OSSIA::AccessMode::BI);
        
        address->setAccessMode(OSSIA::AccessMode::SET);
        QVERIFY(address->getAccessMode() == OSSIA::AccessMode::SET);
        
        QVERIFY(address->getDomain() == nullptr);
        auto domain = Domain::create();
        address->setDomain(domain);
        QVERIFY(address->getDomain() == domain);
        
        QVERIFY(address->getBoundingMode() == OSSIA::BoundingMode::FREE);
        
        address->setBoundingMode(OSSIA::BoundingMode::CLIP);
        QVERIFY(address->getBoundingMode() == OSSIA::BoundingMode::CLIP);
        
        QVERIFY(address->getRepetitionFilter() == false);
        
        address->setRepetitionFilter(true);
        QVERIFY(address->getRepetitionFilter() == true);
        
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

