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
  auto done = minuitDevice->emplace(minuitDevice->begin(), "done");
  done->createAddress(AddressValue::Type::NONE);

  auto test1 = test->emplace(test->begin(), "1");
  test1->createAddress(AddressValue::Type::INT);
  auto test2 = test->emplace(test->begin(), "2");
  test2->createAddress(AddressValue::Type::STRING);
  auto test3 = test->emplace(test->begin(), "3");
  /* todo more to think about Tuples :
  test3->createAddress(AddressValue::Type::INT, AddressValue::Type::INT); */
  auto test4 = test->emplace(test->begin(), "4");
  test4->createAddress(AddressValue::Type::INT);//todo setDomain(0,100)

  OSC PrOSCol{"0.0.0.0", 1000, 1001};
  auto oscDevice = Device::create(PrOSCol);

}
