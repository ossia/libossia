// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
     /test/my_tuple
     */

  auto oscTestNode = device.create_child("test");

  auto oscImpulseNode = oscTestNode->create_child("my_impulse");
  auto oscImpulseAddress = oscImpulseNode->create_parameter(val_type::IMPULSE);
  oscImpulseAddress->add_callback(printValueCallback);

  auto oscBoolNode = oscTestNode->create_child("my_bool");
  auto oscBoolAddress = oscBoolNode->create_parameter(val_type::BOOL);
  oscBoolAddress->add_callback(printValueCallback);

  auto oscIntNode = oscTestNode->create_child("my_int");
  auto oscIntAddress = oscIntNode->create_parameter(val_type::INT);
  oscIntAddress->add_callback(printValueCallback);

  auto oscFloatNode = oscTestNode->create_child("my_float");
  auto oscFloatAddress = oscFloatNode->create_parameter(val_type::FLOAT);
  oscFloatAddress->add_callback(printValueCallback);

  auto oscStringNode = oscTestNode->create_child("my_string");
  auto oscStringAddress = oscStringNode->create_parameter(val_type::STRING);
  oscStringAddress->add_callback(printValueCallback);

  auto oscTupleNode = oscTestNode->create_child("my_tuple");
  auto oscTupleAddress = oscTupleNode->create_parameter(val_type::TUPLE);
  oscTupleAddress->add_callback(printValueCallback);

  // update tree value
  oscImpulseAddress->set_value(impulse{});

  oscBoolAddress->set_value(true);

  oscIntAddress->set_value(5);

  oscFloatAddress->set_value(0.5);

  oscStringAddress->set_value("Hello world!"s);

  oscTupleAddress->set_value(std::vector<ossia::value>{0.1, 0.2, 0.3});

  while (true)
    ;
}

void printValueCallback(const value& v)
{
  cout << "Callback: " << value_to_pretty_string(v) << "\n";
}
