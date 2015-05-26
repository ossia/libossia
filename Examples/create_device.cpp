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
  auto localTestNode = localDevice->emplace(localDevice->children().cend(), "test");
  auto localTestAddress = (*localTestNode)->createAddress(AddressValue::Type::BOOL);

  // Updating local tree value
  Bool b(true);
  localTestAddress->sendValue(&b);

  // Minuit device creation
  Minuit minuitDeviceParameters{"127.0.0.1", 9998, 13579};
  auto minuitDevice = Device::create(minuitDeviceParameters, "newDevice");

  // Minuit tree building
  minuitDevice->updateNamespace();

  // Display tree in console
  std::cout << "\nThe content of Minuit device is : ";
  for(const auto& node : minuitDevice->children())
  {
    std::cout << node->getName() << "\n";
  }
  std::cout << "\n";

  // OSC device creation
  OSC oscDeviceParameters{"127.0.0.1", 9996, 9997};
  auto oscDevice = Device::create(oscDeviceParameters, "oscDevice");

  /* OSC tree building :
      /test
      /test/my_bang
      /test/my_bool
      /test/my_int
      /test/my_float
      /test/my_string
      /test/my_tuple
  */
  auto oscTestNode = oscDevice->emplace(oscDevice->children().cend(), "test");

  auto oscBangNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_bang");
  auto oscBangAddress = (*oscBangNode)->createAddress(AddressValue::Type::NONE);

  auto oscBoolNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_bool");
  auto oscBoolAddress = (*oscBoolNode)->createAddress(AddressValue::Type::BOOL);

  auto oscIntNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_int");
  auto oscIntAddress = (*oscIntNode)->createAddress(AddressValue::Type::INT);

  auto oscFloatNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_float");
  auto oscFloatAddress = (*oscFloatNode)->createAddress(AddressValue::Type::FLOAT);

  auto oscStringNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_string");
  auto oscStringAddress = (*oscStringNode)->createAddress(AddressValue::Type::STRING);

  auto oscTupleNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_tuple");
  // todo : auto oscTupleAddress = (*oscTupleNode)->createAddress(AddressValue::Type::FLOAT, AddressValue::Type::FLOAT, AddressValue::Type::FLOAT);

  // Updating osc tree value
  Int i(0.5);
  oscIntAddress->sendValue(&i);

  while (true)
    ;
}
