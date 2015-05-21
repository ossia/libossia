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

int main()
{
  // No protocol device creation
  Protocol tempDeviceParameters{};
  auto tempDevice = Device::create(tempDeviceParameters, "temp");
  
  // Local device creation
  Local localDeviceParameters{};
  auto localDevice = Device::create(localDeviceParameters, "i-score");
  
  // Local tree building
  auto localTestNode = localDevice->emplace(localDevice->begin(), "test");
  localTestNode->createAddress(AddressValue::Type::BOOL);
  
  // Minuit device creation
  Minuit minuitDeviceParameters{"127.0.0.1", 9998, 13579};
  auto minuitDevice = Device::create(minuitDeviceParameters, "newDevice");

  // Minuit tree building
  minuitDevice->updateNamespace();
/*
  // OSC device creation
  OSC oscDeviceParameters{"127.0.0.1", 9996, 9997};
  auto oscDevice = Device::create(oscDeviceParameters, "oscDevice");

  // OSC tree building
  auto oscTestNode = oscDevice->emplace(oscDevice->begin(), "test");
  oscTestNode->createAddress(AddressValue::Type::BOOL);
 
  auto done = oscDevice->emplace(oscDevice->begin(), "done");
  done->createAddress(AddressValue::Type::NONE);

  auto test1 = test->emplace(test->begin(), "1");
  test1->createAddress(AddressValue::Type::INT);

  auto test2 = test->emplace(test->begin(), "2");
  test2->createAddress(AddressValue::Type::STRING);

  auto test3 = test->emplace(test->begin(), "3");
  // todo: more to think about Tuples
  // test3->createAddress(AddressValue::Type::INT, AddressValue::Type::INT);
  
  auto test4 = test->emplace(test->begin(), "4");
  test4->createAddress(AddressValue::Type::INT); // TODO: setDomain(0,100)
*/

  while (true)
    ;
}
