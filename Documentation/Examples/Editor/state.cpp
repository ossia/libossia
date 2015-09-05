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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol/Local.h"
#include "Network/Protocol/Minuit.h"
#include "Editor/Message.h"
#include "Editor/State.h"

using namespace OSSIA;
using namespace std;

void printValue(const Value * v);
void printValueCallback(const Value * v);

int main()
{
    // Local device
    cout << "\nLocal device\n";
    auto localProtocol = Local::create();
    auto localDevice = Device::create(localProtocol, "i-score");

    // Minuit device creation
    cout << "\nMinuit device\n";
    auto minuitProtocol = Minuit::create("127.0.0.1", 9998, 13579);
    auto minuitDevice = Device::create(minuitProtocol, "newDevice");

    // Minuit tree building
    minuitDevice->updateNamespace();

    // create a state
    auto test = State::create();

    // find bitdepth and samplerateRatio nodes to fill the state
    shared_ptr<Address> bitdepthAddress;
    shared_ptr<Message> bitdepthMessage;
    shared_ptr<Address> samplerateAddress;
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
                    cout << "/deg/bitdepth node found" << endl;

                    bitdepthAddress = parameter->getAddress();
                    bitdepthMessage = Message::create(bitdepthAddress, bitdepthAddress->pullValue());

                    // change the value
                    Int i(10);
                    bitdepthAddress->pushValue(&i);

                    // attach to callback to display later value update
                    bitdepthAddress->addCallback(printValueCallback);
                }
                else if (parameter_name == "samplerate_ratio")
                {
                    cout << "/deg/samplerate_ratio node found" << endl;

                    samplerateAddress = parameter->getAddress();
                    samplerateMessage = Message::create(samplerateAddress, samplerateAddress->pullValue());

                    // change the value
                    Float f(0.5);
                    samplerateAddress->pushValue(&f);

                    // attach to callback to display later value update
                    samplerateAddress->addCallback(printValueCallback);
                }
            }
        }
    }

    cout << "editing and launching state" << endl;

    // fill the state
    test->stateElements().push_back(bitdepthMessage);
    test->stateElements().push_back(samplerateMessage);

    // trigger the message
    test->launch();

    cout << "waiting for /deg/samplerate_ratio > 0.5" << endl;

    // wait while samplerate_ratio > 0.5
    bool wait = true;
    while (wait)
    {
        this_thread::sleep_for( std::chrono::milliseconds(500));

        Float* f = (Float*)samplerateAddress->getValue();
        wait = f->value > 0.5;

        if (!wait)
             cout << "done !" << endl;
    }
}

void printValue(const Value * v)
{
    switch (v->getType())
    {
        case Value::Type::IMPULSE :
        {
            cout << "-";
            break;
        }
        case Value::Type::BOOL :
        {
            Bool * b = (Bool*)v;
            cout << b->value;
            break;
        }
        case Value::Type::INT :
        {
            Int * i = (Int*)v;
            cout << i->value;
            break;
        }
        case Value::Type::FLOAT :
        {
            Float * f = (Float*)v;
            cout << f->value;
            break;
        }
        case Value::Type::CHAR :
        {
            Char * c = (Char*)v;
            cout << c->value;
            break;
        }
        case Value::Type::STRING :
        {
            String * s = (String*)v;
            cout << s->value;
            break;
        }
        case Value::Type::DESTINATION :
        {
            Destination * d = (Destination*)v;
            cout << d->value;
            break;
        }
        case Value::Type::TUPLE :
        {
            Tuple * t = (Tuple*)v;
            bool first = true;
            for (const auto & e : t->value)
            {
                if (!first) cout << " ";
                printValue(e);
                first = false;
            }
            break;
        }
        case Value::Type::GENERIC :
        {
            // todo
            break;
        }
        default:
            break;
    }
}

void printValueCallback(const Value * v)
{
    printValue(v);
    cout << endl;
}
