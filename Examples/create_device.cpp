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
  // Minuit device creation
  Minuit minuitDeviceParameters{"MinuitDevice1", "127.0.0.1", 9998};
  auto minuitDevice = Device::create(minuitDeviceParameters);

  // Minuit tree building
  minuitDevice->updateNamespace();

  // OSC device creation
  OSC oscDeviceParameters{"127.0.0.1", 9996, 9997};
  auto oscDevice = Device::create(oscDeviceParameters);

  // OSC tree building
  auto test = oscDevice->emplace(oscDevice->begin(), "test");
  test->createAddress(AddressValue::Type::BOOL);

  auto done = oscDevice->emplace(oscDevice->begin(), "done");
  done->createAddress(AddressValue::Type::NONE);

  auto test1 = test->emplace(test->begin(), "1");
  test1->createAddress(AddressValue::Type::INT);

  auto test2 = test->emplace(test->begin(), "2");
  test2->createAddress(AddressValue::Type::STRING);

  auto test3 = test->emplace(test->begin(), "3");
  /* TODO: more to think about Tuples
  test3->createAddress(AddressValue::Type::INT, AddressValue::Type::INT);
   */

  auto test4 = test->emplace(test->begin(), "4");
  test4->createAddress(AddressValue::Type::INT); // TODO: setDomain(0,100)
}
