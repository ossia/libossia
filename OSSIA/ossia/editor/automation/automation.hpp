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
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia_export.h>

namespace OSSIA
{
class Value;
class TimeValue;
namespace net { class Address; }
class OSSIA_EXPORT Automation :
    public virtual TimeProcess
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
  static std::shared_ptr<Automation> create(
      net::Address&,
      const Value&);

  /*! destructor */
  virtual ~Automation();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get the address to drive
   \return driven address */
  virtual const net::Address& getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const Value& getDriving() const = 0;
};
}
