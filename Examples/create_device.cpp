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

  Address test = minuitDevice1.addAddress("test", NONE);
  Address done = minuitDevice1.addAddress("done", NONE);

  Address test1 = test.addAddress("1", INT);
  Address test2 = test.addAddress("2", STRING);
  Address test3 = test.addAddress("3", COLOR);
  Address test4 = minuitDevice1.begin()->addAddress("4", INT, 0, 100);

  minuitDevice1.saveNamespace("./namespace.xml");


  Device<OSC> oscDevice(new OSC("0.0.0.0", 1000, 1001));

  oscDevice.loadNamespace("./namespace.xml");

  cout << oscDevice.begin()->getAddress();

}
