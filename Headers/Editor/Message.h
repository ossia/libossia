/*!
 * \file Message.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <memory>

#include "StateElement.h"

namespace OSSIA {

class Address;
class AddressValue;

class Message : public StateElement {

    public:

      // Constructors, destructor, cloning
      static std::shared_ptr<Message> create(std::shared_ptr<Address>,
                                             AddressValue*);
      virtual std::shared_ptr<Message> clone() const = 0;
      virtual ~Message() = default;

      // Execution
      virtual void launch() const override = 0;

      // Accessors
      virtual const std::shared_ptr<Address> & getAddress() const = 0;
      virtual AddressValue * getValue() const = 0;

    };

}
