/*!
 * \file Mapper.h
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
#include <list>

#include "Editor/Clock.h"
#include "Editor/State.h"
#include "Editor/TimeProcess.h"
#include "Misc/Container.h"

namespace OSSIA
{

class Address;
class Value;
class TimeValue;

class Mapper : public virtual TimeProcess
{

public:

# pragma mark -
# pragma mark Life cycle

  /*! factory
   \param the driver address
   \param the driven address
   \param how to map the driver address value on the driven address value
   \return a new mapper */
  static std::shared_ptr<Mapper> create(std::shared_ptr<Address>,
                                        std::shared_ptr<Address>,
                                        const Value*);

  /*! clone */
  virtual std::shared_ptr<Mapper> clone() const = 0;

  /*! destructor */
  virtual ~Mapper();

# pragma mark -
# pragma mark Execution

  /*! get a #Message depending on the parent #TimeConstraint date
   \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> state() override = 0;

# pragma mark -
# pragma mark Accessors

  /*! get the driver address
   \return observed address */
  virtual const std::shared_ptr<Address> getDriverAddress() const = 0;
  
  /*! get the driven address
   \return driven address */
  virtual const std::shared_ptr<Address> getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const Value * getDriving() const = 0;
};
}
