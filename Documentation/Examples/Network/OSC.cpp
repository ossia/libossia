/*!
 * \file OSC.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>
#include <functional>

#include <ossia/ossia.hpp>

using namespace ossia;
using namespace ossia::net;
using namespace std;

void printValueCallback(const value& v);
void printValue(const value& v);

int main()
{
  // declare this program "P" as an OSC device
  ossia::net::generic_device device{
    std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997),
        "P"};

  /* publish each feature of program "P" as address into a tree
     /test
     /test/my_bang
     /test/my_bool
     /test/my_int
     /test/my_float
     /test/my_string
     /test/my_destination
     /test/my_tuple
     */

  auto oscTestNode = device.createChild("test");

  auto oscImpulseNode = oscTestNode->createChild("my_impulse");
  auto oscImpulseAddress = oscImpulseNode->createAddress(val_type::IMPULSE);
  oscImpulseAddress->addCallback(printValueCallback);

  auto oscBoolNode = oscTestNode->createChild("my_bool");
  auto oscBoolAddress = oscBoolNode->createAddress(val_type::BOOL);
  oscBoolAddress->addCallback(printValueCallback);

  auto oscIntNode = oscTestNode->createChild("my_int");
  auto oscIntAddress = oscIntNode->createAddress(val_type::INT);
  oscIntAddress->addCallback(printValueCallback);

  auto oscFloatNode = oscTestNode->createChild("my_float");
  auto oscFloatAddress = oscFloatNode->createAddress(val_type::FLOAT);
  oscFloatAddress->addCallback(printValueCallback);

  auto oscStringNode = oscTestNode->createChild("my_string");
  auto oscStringAddress = oscStringNode->createAddress(val_type::STRING);
  oscStringAddress->addCallback(printValueCallback);

  auto oscDestinationNode = oscTestNode->createChild("my_destination");
  auto oscDestinationAddress = oscDestinationNode->createAddress(val_type::DESTINATION);
  oscDestinationAddress->addCallback(printValueCallback);

  auto oscTupleNode = oscTestNode->createChild("my_tuple");
  auto oscTupleAddress = oscTupleNode->createAddress(val_type::TUPLE);
  oscTupleAddress->addCallback(printValueCallback);

  // update tree value
  oscImpulseAddress->setValue(Impulse{});

  oscBoolAddress->setValue(Bool{true});

  oscIntAddress->setValue(Int{5});

  oscFloatAddress->setValue(Float{0.5});

  oscStringAddress->setValue(String{"Hello world!"});

  oscDestinationAddress->setValue(Destination{*oscFloatNode});

  oscTupleAddress->setValue(Tuple{Float(0.1), Float(0.2), Float(0.3)});

  while (true)
    ;
}

void printValueCallback(const value& v)
{
  cout << "Callback: " << getValueAsString(v) << "\n";
}
