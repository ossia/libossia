/*!
 * \file Node.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef NODE_H
#define NODE_H

#include "Network/AddressValue.h"

#include "Misc/Container.h"

namespace OSSIA {

class Address;

class Node  {

    public:
        using iterator = Container<Node>::iterator;
        using const_iterator = Container<Node>::const_iterator;

        // Destructor
        virtual ~Node() = default;

        // Navigation
        virtual Node & getParent() const = 0;

        // Accessors
        virtual std::string getName() const = 0;
        virtual const std::shared_ptr<Address> & getAddress() const = 0;
        virtual bool removeAddress() = 0;

        // Address Factory
        virtual std::shared_ptr<Address> createAddress(
                AddressValue::Type = AddressValue::Type::NONE) = 0;

        // Child Node Factory
        virtual iterator emplace(const_iterator, std::string) = 0;

        Container<Node>& children()
        { return m_nodes; }
        const Container<Node>& children() const
        { return m_nodes; }

      private:
        Container<Node> m_nodes;
};

}

#endif // NODE_H

