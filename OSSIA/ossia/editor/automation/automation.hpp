#pragma once
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;
class time_value;
namespace net
{
class address_base;
}

/**
 * \brief The automation class
 *
 * An automation is a time process that represents a curve of one
 * or multiple parameters.
 *
 * It drives (i.e. sends messages) to an address, according to a driving value.
 * Its driven \ref net::address_base can be either of numeric type (\ref val_type::INT, \ref val_type::FLOAT...)
 * or of \ref val_type::TUPLE, if the tuple only has numeric elements (e.g. Tuple{Float, Float}).
 *
 * The driving \ref value can either be a single \ref Behavior or a \ref Tuple of \ref Behavior,
 * in accordance to the type of the driven \ref net::address_base.
 *
 * \see \ref Behavior \ref curve \ref curve_segment
 */
class OSSIA_EXPORT automation : public time_process
{
public:
  /** factory
   * \param the address to drive
   * \param how to drive the address
   * \return a new automation
  */
  static std::shared_ptr<automation> create(net::address_base&, const value&);

  virtual ~automation();

  virtual const net::address_base& getDrivenAddress() const = 0;

  virtual const value& getDriving() const = 0;
};
}
