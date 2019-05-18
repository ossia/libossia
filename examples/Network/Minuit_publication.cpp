// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*!
 * \file publication.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <ossia/detail/config.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/value/value.hpp>

#include <iostream>
#include <memory>
#include <functional>


using namespace ossia;
using namespace std;

void printValueCallback(const value& v);
void printValue(const value& v);

int main()
{
  auto local_proto_ptr = std::make_unique<ossia::net::multiplex_protocol>();
  ossia::net::multiplex_protocol& local_proto = *local_proto_ptr;
  // declare this program "B" as Local device
  ossia::net::generic_device device{std::move(local_proto_ptr), "B"};

  /* publish each feature of program "B" as address into a tree
     /test
     /test/my_bang
     /test/my_bool
     /test/my_int
     /test/my_float
     /test/my_string
     /test/my_destination
     /test/my_tuple
     */


  auto localTestNode = device.create_child("test");

  auto localImpulseNode = localTestNode->create_child("my_impulse");
  auto localImpulseAddress = localImpulseNode->create_parameter(val_type::IMPULSE);
  localImpulseAddress->add_callback(printValueCallback);

  auto localBoolNode = localTestNode->create_child("my_bool");
  auto localBoolAddress = localBoolNode->create_parameter(val_type::BOOL);
  localBoolAddress->add_callback(printValueCallback);

  auto localIntNode = localTestNode->create_child("my_int");
  auto localIntAddress = localIntNode->create_parameter(val_type::INT);
  localIntAddress->add_callback(printValueCallback);

  auto localFloatNode = localTestNode->create_child("my_float");
  auto localFloatAddress = localFloatNode->create_parameter(val_type::FLOAT);
  localFloatAddress->add_callback(printValueCallback);

  auto localStringNode = localTestNode->create_child("my_string");
  auto localStringAddress = localStringNode->create_parameter(val_type::STRING);
  localStringAddress->add_callback(printValueCallback);

  // auto localDestinationNode = localTestNode->create_child("my_destination");
  // auto localDestinationAddress = localDestinationNode->create_parameter(val_type::DESTINATION);
  // localDestinationAddress->addCallback(printValueCallback);

  auto localTupleNode = localTestNode->create_child("my_tuple");
  auto localTupleAddress = localTupleNode->create_parameter(val_type::LIST);
  localTupleAddress->add_callback(printValueCallback);

  // update tree value
  localImpulseAddress->push_value(impulse{});
  localBoolAddress->push_value(true);
  localIntAddress->push_value(123);
  localFloatAddress->push_value(0.5);
  localStringAddress->push_value("hello world !"s);

  localTupleAddress->push_value(std::vector<ossia::value>{0., 1., 2.});

  // declare a distant program as a Minuit device
  local_proto.expose_to(std::make_unique<net::minuit_protocol>("B", "127.0.0.1", 9999, 6666));

  while (true)
    ;
}

void printValueCallback(const value& v)
{
  cerr << "Callback: " << value_to_pretty_string(v) << "\n";
}

