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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocols.h"

using namespace OSSIA;

int main() {

  Device<Minuit> minuitDevice1(new Minuit("MinuitDevice1", "1.2.3.4", 66666));

  Address test = minuitDevice1.addAddress<bool>("test");
  Address done = minuitDevice1.addAddress("done");

  Address test1 = test.addAddress<int>("1");
  Address test2 = test.addAddress<std::string>("2");
  Address test3 = test.addAddress<std::pair<int, int>>("3");
  Address test4 = minuitDevice1.begin()->addAddress<int>("4", 0, 100);

  minuitDevice1.save("./namespace.xml");


  Device<OSC> oscDevice(new OSC("0.0.0.0", 1000, 1001));

  oscDevice.load("./namespace.xml");

  std::cout << oscDevice.begin()->getAddress();

}
