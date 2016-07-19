/*!
 * \file exploration.cpp
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
#include "Network/Protocol/Local.h"
#include "Network/Protocol/Minuit.h"

using namespace OSSIA;
using namespace std;

void explore(const shared_ptr<Node> node);
void printValue(const Value * v);
void printDomain(const shared_ptr<Domain> d);
void printValueCallback(const Value * v);

int main()
{
  // declare this program "A" as Local device
  auto localProtocol = Local::create();
  auto localDevice = Device::create(localProtocol, "A");

  // declare a distant program "B" as a Minuit device
  auto minuitProtocol = Minuit::create("127.0.0.1", 6666, 9999);
  auto minuitDevice = Device::create(minuitProtocol, "B");
  {
    // explore the tree of B
    minuitDevice->updateNamespace();

    // display tree in console
    explore(minuitDevice);
  }
}

void explore(const shared_ptr<Node> node)
{
    for (const auto& child : node->children())
    {
        // indentation
        shared_ptr<Node> parent = node->getParent();
        while (parent != nullptr)
        {
            cout << "\t";
            parent = parent->getParent();
        }
        
        cout << child->getName();

        auto address = child->getAddress();

        if (address)
        {
            // attach to callback to display value update
            address->addCallback(printValueCallback);

            // update the value
            address->pullValue();

            // display address info
            cout << " : ";
            switch (address->getValueType())
            {
                case Type::IMPULSE :
                {
                    cout << "Impulse";
                    break;
                }
                case Type::BOOL :
                {
                    cout << "Bool(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::INT :
                {
                    cout << "Int(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::FLOAT :
                {
                    cout << "Float(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::CHAR :
                {
                    cout << "Char(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::STRING :
                {
                    cout << "String(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::DESTINATION :
                {
                    cout << "Destination(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::TUPLE :
                {
                    cout << "Tuple(";
                    printValue(address->getValue());
                    cout << ")";
                    break;
                }
                case Type::GENERIC :
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
                case OSSIA::AccessMode::SET :
                {
                    cout << "set";
                    break;
                }
                case OSSIA::AccessMode::GET :
                {
                    cout << "get";
                    break;
                }
                case OSSIA::AccessMode::BI :
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
                case OSSIA::BoundingMode::FREE :
                {
                    cout << "free";
                    break;
                }
                case OSSIA::BoundingMode::CLIP :
                {
                    cout << "clip";
                    break;
                }
                case OSSIA::BoundingMode::WRAP :
                {
                    cout << "wrap";
                    break;
                }
                case OSSIA::BoundingMode::FOLD :
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

void printValue(const Value * v)
{
    switch (v->getType())
    {
        case Type::IMPULSE :
        {
            cout << "-";
            break;
        }
        case Type::BOOL :
        {
            Bool * b = (Bool*)v;
            cout << b->value;
            break;
        }
        case Type::INT :
        {
            Int * i = (Int*)v;
            cout << i->value;
            break;
        }
        case Type::FLOAT :
        {
            Float * f = (Float*)v;
            cout << f->value;
            break;
        }
        case Type::CHAR :
        {
            Char * c = (Char*)v;
            cout << c->value;
            break;
        }
        case Type::STRING :
        {
            String * s = (String*)v;
            cout << s->value;
            break;
        }
        case Type::DESTINATION :
        {
            Destination * d = (Destination*)v;
            cout << d->value;
            break;
        }
        case Type::TUPLE :
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
        case Type::GENERIC :
        {
            // todo
            break;
        }
        default:
            break;
    }
}

void printDomain(const shared_ptr<Domain> d)
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

void printValueCallback(const Value * v)
{
    printValue(v);
    cout << "\n";
}
