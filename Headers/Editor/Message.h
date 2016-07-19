/*!
 * \file Message.h
 *
 * \defgroup Editor
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
#include <Editor/Value/Value.h>
#include <ossia_export.h>

namespace OSSIA
{

class Address;
class Value;

class OSSIA_EXPORT Message : public virtual StateElement
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param std::shared_ptr<#Address> where to send the value
   \param const #Value the value
   \return std::shared_ptr<#Message> */
  static std::shared_ptr<Message> create(std::shared_ptr<Address>, const Value&);

  /*! clone */
  virtual std::shared_ptr<Message> clone() const = 0;

  /*! destructor */
  virtual ~Message();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! send the value to the address */
  // TODO what is the point of this if launch() is already in the parent class?
  virtual void launch() const override = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get message's address
   \return std::shared_ptr<#Address> the address */
  virtual const std::shared_ptr<Address> & getAddress() const = 0;

  /*! get message's value
   \return #Value the value */
  virtual const Value * getValue() const = 0;
};

}
