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
#include <ossia_export.h>

namespace OSSIA
{

class Address;
class Value;
class TimeValue;

class OSSIA_EXPORT Automation : public virtual TimeProcess
{

public:
#if 0
# pragma mark -
# pragma mark Life cycle
#endif
  /*! factory
   \param the address to drive
   \param how to drive the address
   \return a new automation */
  static std::shared_ptr<Automation> create(std::shared_ptr<Address>,
                                            const Value*);

  /*! clone */
  virtual std::shared_ptr<Automation> clone() const = 0;

  /*! destructor */
  virtual ~Automation();

#if 0
# pragma mark -
# pragma mark Execution
#endif

  /*! get a #Message depending on the parent #TimeConstraint offset
   \details don't call offset when the parent #TimeConstraint is running
   \param const #TimeValue offset date
   \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> offset(const TimeValue&) override = 0;

  /*! get a #Message depending on the parent #TimeConstraint date
   \details don't call state when the parent #TimeConstraint is not running
   \return std::shared_ptr<#StateElement> */
  virtual std::shared_ptr<StateElement> state() override = 0;

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the address to drive
   \return driven address */
  virtual const std::shared_ptr<Address> getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const Value * getDriving() const = 0;
};
}
