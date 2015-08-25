#include <QtTest>
#include "../ForwardDeclaration.h"
#include <iostream>

using namespace OSSIA;

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
        
        QVERIFY(address->getValueType() == Value::Type::IMPULSE);
        
        //! \todo verify setValueType
        
        QVERIFY(address->getAccessMode() == Address::AccessMode::BI);
        
        address->setAccessMode(Address::AccessMode::SET);
        QVERIFY(address->getAccessMode() == Address::AccessMode::SET);
        
        QVERIFY(address->getDomain() != nullptr);
        
        //! \todo verify setDomain
        
        QVERIFY(address->getBoundingMode() == Address::BoundingMode::FREE);
        
        address->setBoundingMode(Address::BoundingMode::CLIP);
        QVERIFY(address->getBoundingMode() == Address::BoundingMode::CLIP);
        
        QVERIFY(address->getRepetitionFilter() == false);
        
        address->setRepetitionFilter(true);
        QVERIFY(address->getRepetitionFilter() == true);
        
        //! \todo verify getValueCallback
        
        //! \todo verify setValueCallback
        
        //! \todo verify getValue
        
        //! \todo verify sendValue
    }
};

QTEST_APPLESS_MAIN(AddressTest)

#include "AddressTest.moc"

