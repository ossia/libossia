#pragma once
#include <ossia/detail/ptr_container.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
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
 * The automation has a "source" domain, i.e. the data space in which the transformation
 * happens. The target domain is taken from the driven address_base.
 * The unit is stored in mLastMessage.unit.
 *
 *
 * \see \ref Behavior \ref curve \ref curve_segment
 */
class OSSIA_EXPORT automation final :
    public ossia::time_process
{
  public:
    automation(Destination, const ossia::value&);
    automation(Destination, ossia::value&&);
    automation(const automation&);

    ~automation();

    Destination getDrivenAddress() const;
    const ossia::value& getDriving() const;

  private:
    ossia::state_element offset(ossia::time_value) override;
    ossia::state_element state() override;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;

    static ossia::value computeValue(double, const ossia::value&);

    void updateMessage(double t);

    Destination mDrivenAddress;
    ossia::value mDrive;
    ossia::message mLastMessage;
};
}
