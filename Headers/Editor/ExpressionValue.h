/*!
 * \file ExpressionValue.h
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

namespace OSSIA {

class ExpressionValue {

    public:
      enum class Type { Address, Value };
      virtual ~ExpressionValue() = default;
      
      // What is currently in the union
      virtual Type getCurrentType() const = 0;
      
      // If the ExpressionValue is an address, then it is returned, else an empty shared_ptr
      virtual std::shared_ptr<Address> getAddress() const = 0;
      
      // If the ExpressionValue is a fixed value, then it is returned, else an empty shared_ptr
      virtual std::shared_ptr<AddressValue> getValue() const = 0;
      

    };

}
