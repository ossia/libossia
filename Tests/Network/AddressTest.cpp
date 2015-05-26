#include <QtTest>

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol.h"


using namespace OSSIA;

class AddressTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void addressMainTest()
    {
        Local loc{};
        auto localDev = Device::create(loc, "localDev");
    }
};

QTEST_APPLESS_MAIN(AddressTest)

#include "AddressTest.moc"

