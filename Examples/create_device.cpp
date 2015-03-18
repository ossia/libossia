/*!
 * \file create_device.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol.h"

using namespace OSSIA;
using namespace std;

int main() {

  Minuit myProt{"MinuitDevice1", "1.2.3.4", 66666};
  auto minuitDevice = Device::create(myProt);

  auto test = minuitDevice->emplace(minuitDevice->begin(), "test");
  test->createAddress(AddressValue::Type::BOOL);
  auto done = minuitDevice->addAddress("done");

  auto test1 = test->addAddress("1", AddressType::INT);
  auto test2 = test->addAddress("2", AddressType::STRING);
  auto test3 = test->addAddress("3", AddressType::INT, AddressType::INT);
  auto test4 = test->addAddress("4", AddressType::INT, 0, 100);

  OSC PrOSCol("0.0.0.0", 1000, 1001);
  auto oscDevice = Device.create(PrOSCol);

}
