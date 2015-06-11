/*!
 * \file network.cpp
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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol.h"

using namespace OSSIA;
using namespace std;

void explore(shared_ptr<Node> node);
void printValue(const AddressValue * v);
void printDomain(const AddressDomain * d);
void printValueCallback(const AddressValue * v);

int main()
{
  // No protocol device creation
  Protocol tempDeviceParameters{};
  auto tempDevice = Device::create(tempDeviceParameters, "temp");

  // Local device
  cout << "\nLocal device example\n";
  Local localDeviceParameters{};
  auto localDevice = Device::create(localDeviceParameters, "i-score");
/*  {
    // tree building
    auto localTestNode = localDevice->emplace(localDevice->children().cend(), "test");
    auto localTestAddress = (*localTestNode)->createAddress(AddressValue::Type::BOOL);
    
    // updating local tree value
    Bool b(true);
    localTestAddress->sendValue(&b);
  }
*/
  // Minuit device
  cout << "\nMinuit device example\n";
  Minuit minuitDeviceParameters{"127.0.0.1", 9998, 13579};
  auto minuitDevice = Device::create(minuitDeviceParameters, "MinuitDevice");
  {
    // tree building
    minuitDevice->updateNamespace();
    
    // display tree in console and attached to callback to display value update)
    cout << "The content of Minuit device is :\n";
    explore(minuitDevice);
  }

  // OSC device
  cout << "\nOSC device example\n";
  OSC oscDeviceParameters{"127.0.0.1", 9996, 9997};
  auto oscDevice = Device::create(oscDeviceParameters, "oscDevice");
  {
    OSSIA::Address::ValueCallback callback = printValueCallback;
    /* tree building :
     /test
     /test/my_bang
     /test/my_bool
     /test/my_int
     /test/my_float
     /test/my_string
     /test/my_tuple
    */
    auto oscTestNode = oscDevice->emplace(oscDevice->children().cend(), "test");
    
    auto oscImpulseNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_impulse");
    auto oscImpulseAddress = (*oscImpulseNode)->createAddress(AddressValue::Type::IMPULSE);
    oscImpulseAddress->setValueCallback(callback);
    
    auto oscBoolNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_bool");
    auto oscBoolAddress = (*oscBoolNode)->createAddress(AddressValue::Type::BOOL);
    oscBoolAddress->setValueCallback(callback);
    
    auto oscIntNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_int");
    auto oscIntAddress = (*oscIntNode)->createAddress(AddressValue::Type::INT);
    oscIntAddress->setValueCallback(callback);
    
    auto oscFloatNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_float");
    auto oscFloatAddress = (*oscFloatNode)->createAddress(AddressValue::Type::FLOAT);
    oscFloatAddress->setValueCallback(callback);
    
    auto oscStringNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_string");
    auto oscStringAddress = (*oscStringNode)->createAddress(AddressValue::Type::STRING);
    oscStringAddress->setValueCallback(callback);
    
    auto oscTupleNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_tuple");
    auto oscTupleAddress = (*oscTupleNode)->createAddress(AddressValue::Type::TUPLE);
    oscTupleAddress->setValueCallback(callback);
    
    // updating tree value
    Impulse n;
    oscImpulseAddress->sendValue(&n);
    
    Bool b(true);
    oscBoolAddress->sendValue(&b);
    
    Int i(5);
    oscIntAddress->sendValue(&i);
    
    Float f(0.5);
    oscFloatAddress->sendValue(&f);
    
    String s("hello world !");
    oscStringAddress->sendValue(&s);
      
    std::vector<AddressValue*> value = {new Float(0.1), new Float(0.2), new Float(0.3)};
    Tuple t(value);
    oscTupleAddress->sendValue(&t);
  }

  while (true)
    ;
}

void explore(shared_ptr<Node> node)
{
    for (const auto& child : node->children())
    {
        cout << child->getName();
        
        auto address = child->getAddress();
        
        if (address)
        {
            // attach to callback
            OSSIA::Address::ValueCallback callback = printValueCallback;
            address->setValueCallback(callback);
            
            // display address info
            cout << " : ";
            switch (address->getValueType())
            {
                case AddressValue::Type::IMPULSE :
                {
                    cout << "Impulse";
                    break;
                }
                case AddressValue::Type::BOOL :
                {
                    cout << "Bool(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case AddressValue::Type::INT :
                {
                    cout << "Int(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case AddressValue::Type::FLOAT :
                {
                    cout << "Float(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case AddressValue::Type::CHAR :
                {
                    cout << "Char(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case AddressValue::Type::STRING :
                {
                    cout << "String(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case AddressValue::Type::TUPLE :
                {
                    cout << "Tuple(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case AddressValue::Type::GENERIC :
                {
                    cout << "Generic(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                default:
                    break;
            }
            
            cout << ", AccessMode(";
            switch (address->getAccessMode())
            {
                case Address::AccessMode::SET :
                {
                    cout << "set";
                    break;
                }
                case Address::AccessMode::GET :
                {
                    cout << "get";
                    break;
                }
                case Address::AccessMode::BI :
                {
                    cout << "bi";
                    break;
                }
                default:
                    break;
            }
            
            cout << "), BoundingMode(";
            switch (address->getBoundingMode())
            {
                case Address::BoundingMode::FREE :
                {
                    cout << "free";
                    break;
                }
                case Address::BoundingMode::CLIP :
                {
                    cout << "clip";
                    break;
                }
                case Address::BoundingMode::WRAP :
                {
                    cout << "wrap";
                    break;
                }
                case Address::BoundingMode::FOLD :
                {
                    cout << "fold";
                    break;
                }
                default:
                    break;
            }
            
            cout << "), Domain(";
            printDomain(address->getDomain());
            cout << ")";
        }
        
        cout << "\n";
        
        explore(child);
    }
}

void printValue(const AddressValue * v)
{
    switch (v->getType())
    {
        case AddressValue::Type::IMPULSE :
        {
            cout << "-";
            break;
        }
        case AddressValue::Type::BOOL :
        {
            Bool * b = (Bool*)v;
            cout << b->value;
            break;
        }
        case AddressValue::Type::INT :
        {
            Int * i = (Int*)v;
            cout << i->value;
            break;
        }
        case AddressValue::Type::FLOAT :
        {
            Float * f = (Float*)v;
            cout << f->value;
            break;
        }
        case AddressValue::Type::CHAR :
        {
            Char * c = (Char*)v;
            cout << c->value;
            break;
        }
        case AddressValue::Type::STRING :
        {
            String * s = (String*)v;
            cout << s->value;
            break;
        }
        case AddressValue::Type::TUPLE :
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
        case AddressValue::Type::GENERIC :
        {
            // todo
            break;
        }
        default:
            break;
    }
}

void printDomain(const AddressDomain * d)
{
    printValue(d->getMin());
    cout << ", ";
    printValue(d->getMax());
    if (!d->getValues().empty())
    {
        cout << ", ";
        for (const auto & v : d->getValues())
            printValue(v);
    }
}

void printValueCallback(const AddressValue * v)
{
    printValue(v);
    cout << "\n";
}