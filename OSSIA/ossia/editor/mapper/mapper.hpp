#pragma once
#include <list>

#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia_export.h>

namespace ossia
{
namespace net
{
class address_base;
}
class value;
class time_value;

class OSSIA_EXPORT mapper : public virtual time_process
{

public:
  /*! factory
   \param the driver address
   \param the driven address
   \param how to map the driver address value on the driven address value
   \return a new mapper */
  static std::shared_ptr<mapper>
  create(net::address_base&, net::address_base&, const value&);

  /*! destructor */
  virtual ~mapper();

  /*! get the driver address
   \return observed address */
  virtual const net::address_base& getDriverAddress() const = 0;

  /*! get the driven address
   \return driven address */
  virtual const net::address_base& getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const value& getDriving() const = 0;
};
}
