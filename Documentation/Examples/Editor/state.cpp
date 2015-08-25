/*!
 * \file state.cpp
 *
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol/Local.h"
#include "Network/Protocol/Minuit.h"
#include "Editor/Message.h"
#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

int main()
{
    // Local device
    cout << "\nLocal device\n";
    Local localDeviceParameters{};
    auto localDevice = Device::create(localDeviceParameters, "i-score");

    // Minuit device creation
    cout << "\nMinuit device\n";
    Minuit minuitDeviceParameters{"127.0.0.1", 9998, 13579};
    auto minuitDevice = Device::create(minuitDeviceParameters, "newDevice");

    // Minuit tree building
    minuitDevice->updateNamespace();

    // create a state
    auto test = State::create();

    // find bitdepth and samplerateRatio nodes to fill the state
    shared_ptr<Message> bitdepthMessage;
    shared_ptr<Message> samplerateMessage;

    for (const auto& module : minuitDevice->children())
    {
        if (module->getName() == "deg")
        {
            for (const auto& parameter : module->children())
            {
                string parameter_name = parameter->getName();

                if (parameter_name == "bitdepth")
                {
                    cout << "\n/deg/bitdepth node found\n";

                    auto bitdepthAddress = parameter->getAddress();
                    bitdepthMessage = Message::create(bitdepthAddress, bitdepthAddress->getValue());

                    // change the value
                    Int i(10);
                    bitdepthAddress->sendValue(&i);
                }
                else if (parameter_name == "samplerateRatio")
                {
                    cout << "\n/deg/samplerateRatio node found\n";

                    auto samplerateAddress = parameter->getAddress();
                    samplerateMessage = Message::create(samplerateAddress, samplerateAddress->getValue());

                    // change the value
                    Float f(0.5);
                    samplerateAddress->sendValue(&f);
                }
            }
        }
    }

    // fill the state
    test->stateElements().push_back(bitdepthMessage);
    test->stateElements().push_back(samplerateMessage);

    // trigger the message
    test->launch();
}
