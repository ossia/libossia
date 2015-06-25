/*!
 * \file Message.h
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

#include <memory>

#include "StateElement.h"

namespace OSSIA
{

class Address;
class Value;

class Message : public StateElement
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param std::shared_ptr<#Address> where to send the value
   \param const #Value the value
   \return std::shared_ptr<#Message> */
  static std::shared_ptr<Message> create(std::shared_ptr<Address>,
                                         const Value*);
  
  /*! clone */
  virtual std::shared_ptr<Message> clone() const = 0;
  
  /*! destructor */
  virtual ~Message() = default;

# pragma mark -
# pragma mark Execution
  
  /*! send the value to the address */
  virtual void launch() const override = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get message's address
   \return std::shared_ptr<#Address> the address */
  virtual const std::shared_ptr<Address> & getAddress() const = 0;
  
  /*! get message's value
   \return #Value the value */
  virtual const Value * getValue() const = 0;
};

}
