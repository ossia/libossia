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
#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

int main()
{
    // Minuit device creation
    cout << "\nMinuit device\n";
    Minuit minuitDeviceParameters{"127.0.0.1", 9998, 13579};
    auto minuitDevice = Device::create(minuitDeviceParameters, "newDevice");

    // Minuit tree building
    minuitDevice->updateNamespace();

    // state creation
    shared_ptr<State> s = State::create();

    s->stateElements().insert(s->stateElements().begin(), State::create());

    cout<<"done!"<<endl;
}
