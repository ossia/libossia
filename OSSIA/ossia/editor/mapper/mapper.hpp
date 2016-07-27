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

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia_export.h>

namespace OSSIA
{

class Address;
class Value;
class TimeValue;

class OSSIA_EXPORT Mapper : public virtual TimeProcess
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param the driver address
   \param the driven address
   \param how to map the driver address value on the driven address value
   \return a new mapper */
  static std::shared_ptr<Mapper> create(std::shared_ptr<Address>,
                                        std::shared_ptr<Address>,
                                        const Value&);

  /*! clone */
  virtual std::shared_ptr<Mapper> clone() const = 0;

  /*! destructor */
  virtual ~Mapper();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the driver address
   \return observed address */
  virtual const std::shared_ptr<Address> getDriverAddress() const = 0;

  /*! get the driven address
   \return driven address */
  virtual const std::shared_ptr<Address> getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const Value& getDriving() const = 0;
};
}
