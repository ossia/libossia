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
  device.getProtocol().update(device.getRootNode());

  // create a state
  state test;

  // find bitdepth and samplerateRatio nodes to fill the state
  address_base* bitdepthAddress;
  boost::optional<message> bitdepthMessage;
  address_base* samplerateAddress;
  boost::optional<message> samplerateMessage;

  for (const auto& module : device.children())
  {
    if (module->getName() == "deg")
    {
      for (const auto& parameter : module->children())
      {
        string parameter_name = parameter->getName();

        if (parameter_name == "bitdepth")
        {
          cout << "/deg/bitdepth node found" << endl;

          bitdepthAddress = parameter->getAddress();
          bitdepthAddress->pullValue();
          bitdepthMessage = message{*bitdepthAddress, bitdepthAddress->cloneValue()};

          // change the value
          bitdepthAddress->pushValue(Int(10));

          // attach to callback to display later value update
          bitdepthAddress->add_callback(printValueCallback);
        }
        else if (parameter_name == "samplerate_ratio")
        {
          cout << "/deg/samplerate_ratio node found" << endl;

          samplerateAddress = parameter->getAddress();
          samplerateAddress->pullValue();
          samplerateMessage = message{*samplerateAddress, samplerateAddress->cloneValue()};

          // change the value
          samplerateAddress->pushValue(Float(0.5));

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

    auto f = samplerateAddress->cloneValue().get<Float>();
    wait = f > 0.5;

    if (!wait)
      cout << "done !" << endl;
  }
}


void printValueCallback(const value& v)
{
  cout << "Callback: " << value_to_pretty_string(v) << "\n";
}
