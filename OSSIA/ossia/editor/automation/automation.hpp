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

#include "Editor/Clock.hpp"
#include "Editor/State.hpp"
#include "Editor/TimeProcess.hpp"
#include "Misc/Container.hpp"
#include <ossia_export.h>

namespace OSSIA
{
class Value;
class Address;
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
                                            const Value&);

  /*! clone */
  virtual std::shared_ptr<Automation> clone() const = 0;

  /*! destructor */
  virtual ~Automation();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the address to drive
   \return driven address */
  virtual const std::shared_ptr<Address> getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const Value& getDriving() const = 0;
};
}
