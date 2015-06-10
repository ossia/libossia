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

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Protocol.h"

using namespace OSSIA;
using namespace std;

void explore(shared_ptr<Node> node);

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
  auto minuitDevice = Device::create(minuitDeviceParameters, "newDevice");
  {
    // tree building
    minuitDevice->updateNamespace();
    
    // display tree in console
    cout << "The content of Minuit device is :\n";
    explore(minuitDevice);
  }

  // OSC device
  cout << "\nOSC device example\n";
  OSC oscDeviceParameters{"127.0.0.1", 9996, 9997};
  auto oscDevice = Device::create(oscDeviceParameters, "oscDevice");
  {
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
    
    auto oscBangNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_bang");
    auto oscBangAddress = (*oscBangNode)->createAddress(AddressValue::Type::NONE);
    
    auto oscBoolNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_bool");
    auto oscBoolAddress = (*oscBoolNode)->createAddress(AddressValue::Type::BOOL);
    
    auto oscIntNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_int");
    auto oscIntAddress = (*oscIntNode)->createAddress(AddressValue::Type::INT);
    
    auto oscFloatNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_float");
    auto oscFloatAddress = (*oscFloatNode)->createAddress(AddressValue::Type::FLOAT);
    
    auto oscStringNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_string");
    auto oscStringAddress = (*oscStringNode)->createAddress(AddressValue::Type::STRING);
    
    auto oscTupleNode = (*oscTestNode)->emplace((*oscTestNode)->children().cend(), "my_tuple");
    // todo : auto oscTupleAddress = (*oscTupleNode)->createAddress(AddressValue::Type::FLOAT, AddressValue::Type::FLOAT, AddressValue::Type::FLOAT);
    
    // updating tree value
    None n;
    oscBangAddress->sendValue(&n);
    
    Bool b(true);
    oscBoolAddress->sendValue(&b);
    
    Int i(5);
    oscIntAddress->sendValue(&i);
    
    Float f(0.5);
    oscFloatAddress->sendValue(&f);
    
    String s("hello world !");
    oscStringAddress->sendValue(&s);
  }

  while (true)
    ;
}

void explore(shared_ptr<Node> node)
{
    for(const auto& child : node->children())
    {
        cout << child->getName();
        
        auto address = child->getAddress();
        
        if (address)
        {
            cout << " : address type=";
            switch (address->getValueType())
            {
                case AddressValue::Type::NONE :
                {
                    cout << "none";
                    break;
                }
                case AddressValue::Type::BOOL :
                {
                    cout << "bool";
                    Bool * b = (Bool*)address->getValue();
                    cout << ", value=< " << b->value << " >";
                    break;
                }
                case AddressValue::Type::INT :
                {
                    cout << "int";
                    Int * i = (Int*)address->getValue();
                    cout << ", value=< " << i->value << " >";
                    break;
                }
                case AddressValue::Type::FLOAT :
                {
                    cout << "float";
                    Float * f = (Float*)address->getValue();
                    cout << ", value=< " << f->value << " >";
                    break;
                }
                case AddressValue::Type::CHAR :
                {
                    cout << "char";
                    Char * c = (Char*)address->getValue();
                    cout << ", value=< " << c->value << " >";
                    break;
                }
                case AddressValue::Type::STRING :
                {
                    cout << "string";
                    String * s = (String*)address->getValue();
                    cout << ", value=< " << s->value << " >";
                    break;
                }
                case AddressValue::Type::TUPLE :
                {
                    cout << "tuple";
                    Tuple * t = (Tuple*)address->getValue();
                    
                    cout << ", value=<";
                    for (const auto & e : t->value)
                    {
                        cout << " ";
                        switch (e->getType())
                        {
                            case AddressValue::Type::NONE :
                            {
                                cout << "-";
                                break;
                            }
                            case AddressValue::Type::BOOL :
                            {
                                Bool * b = (Bool*)e;
                                cout << b->value;
                                break;
                            }
                            case AddressValue::Type::INT :
                            {
                                Int * i = (Int*)e;
                                cout << i->value;
                                break;
                            }
                            case AddressValue::Type::FLOAT :
                            {
                                Float * f = (Float*)e;
                                cout << f->value;
                                break;
                            }
                            case AddressValue::Type::CHAR :
                            {
                                Char * c = (Char*)e;
                                cout << c->value;
                                break;
                            }
                            case AddressValue::Type::STRING :
                            {
                                String * s = (String*)e;
                                cout << s->value;
                                break;
                            }
                            case AddressValue::Type::TUPLE :
                            {
                                // todo : make a recursive conversion function
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
                    cout << " >";
                        
                    break;
                }
                case AddressValue::Type::GENERIC :
                {
                    cout << "generic";
                    // todo: cout << ", value=" << address->getValue();
                    break;
                }
                default:
                    break;
            }
            
            cout << ", access=";
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
            
            cout << ", bounding=";
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
        }
        
        cout << "\n";
        
        explore(child);
    }
}
