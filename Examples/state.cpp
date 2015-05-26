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
#include "Network/Protocol.h"
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
    
    // Find bitdepth node to create a bitdepth message
    shared_ptr<Message> bitdepthMessage;
    for (const auto& module : minuitDevice->children())
    {
        if (module->getName() == "deg")
        {
            for (const auto& parameter : module->children())
            {
                if (parameter->getName() == "bitdepth")
                {
                    cout << "\n/deg/bitdepth node found\n";
                    
                    auto bitdepthAddress = parameter->getAddress();
                    bitdepthMessage = Message::create(bitdepthAddress, bitdepthAddress->getValue());
                    
                    // change the value
                    Int i(10);
                    bitdepthAddress->sendValue(&i);
                    
                    break;
                }
            }
        }
    }
    
    // trigger the message
    bitdepthMessage->launch();
}
