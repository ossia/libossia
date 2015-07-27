/*!
 * \file Automation.h
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

class Automation : public virtual TimeProcess
{
  
public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param the address to drive
   \param how to drive the address
   \return a new automation */
  static std::shared_ptr<Automation> create(std::shared_ptr<Address>,
                                            const Value*);
  
  /*! clone */
  virtual std::shared_ptr<Automation> clone() const = 0;
  
  /*! destructor */
  virtual ~Automation() = default;
  
# pragma mark -
# pragma mark Execution
  
  /*! get a #Message for a position or a date
   \param const #TimeValue position
   \param const #TimeValue date
   \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> state(const TimeValue&, const TimeValue&) = 0;
  
  /*! \todo record */
  
# pragma mark -
# pragma mark Accessors
  
  /*! get the address to drive
   \return driven address */
  virtual const std::shared_ptr<Address> getDrivenAddress() const = 0;
  
  /*! get the driving value
   \return driving value */
  virtual const Value * getDriving() const = 0;
};
}
