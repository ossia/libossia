// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*!
 * \file state.cpp
 *
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <thread>
#include <memory>

#include <ossia/ossia.hpp>
using namespace ossia;
using namespace ossia::net;
using namespace std;

void printValue(const value& v);
void printValueCallback(const value& v);

int main()
{
  // Local device
  ossia::net::generic_device device{
         std::make_unique<ossia::net::minuit_protocol>("i-score", "127.0.0.1", 9998, 13579),
        "newDevice"};

  // Minuit tree building
  device.get_protocol().update(device.get_root_node());

  // create a state
  state test;

  // find bitdepth and samplerateRatio nodes to fill the state
  parameter_base* bitdepthAddress{};
  ossia::optional<message> bitdepthMessage;
  parameter_base* samplerateAddress{};
  ossia::optional<message> samplerateMessage;

  for (const auto& module : device.children())
  {
    if (module->get_name() == "deg")
    {
      for (const auto& parameter : module->children())
      {
        string parameter_name = parameter->get_name();

        if (parameter_name == "bitdepth")
        {
          cout << "/deg/bitdepth node found" << endl;

          bitdepthAddress = parameter->get_address();
          bitdepthAddress->pull_value();
          bitdepthMessage = message{*bitdepthAddress, bitdepthAddress->value()};

          // change the value
          bitdepthAddress->push_value(10);

          // attach to callback to display later value update
          bitdepthAddress->add_callback(printValueCallback);
        }
        else if (parameter_name == "samplerate_ratio")
        {
          cout << "/deg/samplerate_ratio node found" << endl;

          samplerateAddress = parameter->get_address();
          samplerateAddress->pull_value();
          samplerateMessage = message{*samplerateAddress, samplerateAddress->value()};

          // change the value
          samplerateAddress->push_value(0.5);

          // attach to callback to display later value update
          samplerateAddress->add_callback(printValueCallback);
        }
      }
    }
  }

  if(!samplerateAddress)
    return 0;

  if(!bitdepthAddress)
    return 0;

  cout << "editing and launching state" << endl;

  // fill the state
  test.add(bitdepthMessage);
  test.add(samplerateMessage);

  // trigger the message
  test.launch();

  cout << "waiting for /deg/samplerate_ratio > 0.5" << endl;

  // wait while samplerate_ratio > 0.5
  bool wait = true;
  while (wait)
  {
    this_thread::sleep_for( std::chrono::milliseconds(500));

    auto f = samplerateAddress->value().get<float>();
    wait = f > 0.5;

    if (!wait)
      cout << "done !" << endl;
  }
}


void printValueCallback(const value& v)
{
  cout << "Callback: " << value_to_pretty_string(v) << "\n";
}
