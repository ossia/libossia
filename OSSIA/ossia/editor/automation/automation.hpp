#pragma once
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/detail/ptr_container.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;
class time_value;
namespace net { class address_base; }
class OSSIA_EXPORT automation :
    public virtual time_process
{
public:
  /*! factory
   \param the address to drive
   \param how to drive the address
   \return a new automation */
  static std::shared_ptr<automation> create(
      net::address_base&,
      const value&);

  /*! destructor */
  virtual ~automation();

  /*! get the address to drive
   \return driven address */
  virtual const net::address_base& getDrivenAddress() const = 0;

  /*! get the driving value
   \return driving value */
  virtual const value& getDriving() const = 0;
};
}
