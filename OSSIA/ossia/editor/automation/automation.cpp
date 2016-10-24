#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/base/address.hpp>
#include <boost/container/static_vector.hpp>
#include <iostream>

namespace ossia
{

automation::automation(
    Destination address, const ossia::behavior& drive)
  : mDrivenAddress(address)
  , mDrive(drive)
  , mLastMessage{address, ossia::value{}}
{
}

automation::automation(
    Destination address, ossia::behavior&& drive)
  : mDrivenAddress(address)
  , mDrive(std::move(drive))
  , mLastMessage{address, ossia::value{}}
{
}

automation::~automation() = default;

void automation::updateMessage(double t)
{
  mLastMessage.message_value = computeValue(t, mDrive);
}

ossia::state_element automation::offset(ossia::time_value offset)
{
  auto& par = *parent();
  if (par.getRunning())
  {
    throw execution_error("automation_impl::offset: "
                          "parent time constraint is running");
    return {};
  }
  // edit a Message handling the new Value
  updateMessage(offset / par.getDurationNominal());
  return mLastMessage;
}

ossia::state_element automation::state()
{
  auto& par = *parent();
  if (par.getRunning())
  {
    // if date hasn't been processed already
    ossia::time_value date = par.getDate();
    if (date != mLastDate)
    {
      mLastDate = date;

      // edit a Message handling the new Value
      updateMessage(par.getDate() / par.getDurationNominal());
    }

    return mLastMessage;
  }
  else
  {
    throw execution_error("automation_impl::state: "
                          "parent time constraint is not running");
    return {};
  }
}

void automation::start()
{
  mDrive.reset();
}

void automation::stop()
{
}

void automation::pause()
{
}

void automation::resume()
{
}

Destination automation::getDrivenAddress() const
{
  return mDrivenAddress;
}

const ossia::behavior& automation::getDriving() const
{
  return mDrive;
}

struct computeValue_visitor
{
  double position;

  ossia::value error() const
  {
    throw invalid_value_type_error("computeValue_visitor: "
                                   "Unhandled drive value type.");
    return {};
  }

  template<typename T>
  ossia::value operator()(const T&) const
  {
    return error();
  }

  ossia::value operator()() const
  {
    return error();
  }

  ossia::value operator()(const curve_ptr& c) const
  {
    auto base_curve = c.get();
    if(!base_curve)
    {
      throw invalid_value_type_error("computeValue_visitor: "
                                     "invalid Behavior");
      return {};
    }

    auto t = base_curve->getType();
    if (t.first == ossia::curve_segment_type::DOUBLE)
    {
      switch (t.second)
      {
        case ossia::curve_segment_type::FLOAT:
        {
          auto c = static_cast<curve<double, float>*>(base_curve);
          return ossia::Float{c->valueAt(position)};
        }
        case ossia::curve_segment_type::INT:
        {
          auto c = static_cast<curve<double, int>*>(base_curve);
          return ossia::Int{c->valueAt(position)};
        }
        case ossia::curve_segment_type::BOOL:
        {
          auto c = static_cast<curve<double, bool>*>(base_curve);
          return ossia::Bool{c->valueAt(position)};
        }
        case ossia::curve_segment_type::DOUBLE:
          break;
        case ossia::curve_segment_type::ANY:
        {
          auto c = static_cast<constant_curve*>(base_curve);
          // TODO we need a specific handling for destination.
          return c->value();
        }
      }
    }


    throw invalid_value_type_error("computeValue_visitor: "
                                   "drive curve type is not DOUBLE");
    return {};
  }

  using vec_behavior = boost::container::static_vector<curve<double, float>*, 4>;
  static vec_behavior tuple_convertible_to_vec(
      const std::vector<ossia::behavior>& t)
  {
    const auto n = t.size();

    boost::container::static_vector<curve<double, float>*, 4> arr;
    bool ok = false;
    if(n >= 2 && n <= 4)
    {
      for(const ossia::behavior& v : t)
      {
        auto target = v.target<std::shared_ptr<ossia::curve_abstract>>();
        if(!target)
          return {};

        ossia::curve_abstract* c = target->get();
        if(!c)
          return {};

        auto t = c->getType();
        if(t.first == ossia::curve_segment_type::DOUBLE && t.second == ossia::curve_segment_type::FLOAT)
        {
          arr.push_back(static_cast<ossia::curve<double,float>*>(c));
          ok = true;
          continue;
        }
        else
        {
          ok = false;
          break;
        }
      }

      if(ok)
        return arr;
      else
        return {};
    }
    return {};
  }

  ossia::value operator()(const std::vector<ossia::behavior>& b)
  {
    vec_behavior arr = tuple_convertible_to_vec(b);

    if(!arr.empty())
    {
      // VecNf case.
      switch(arr.size())
      {
        case 2:
          return ossia::make_vec(arr[0]->valueAt(position), arr[1]->valueAt(position));
        case 3:
          return ossia::make_vec(arr[0]->valueAt(position), arr[1]->valueAt(position), arr[2]->valueAt(position));
        case 4:
          return ossia::make_vec(arr[0]->valueAt(position), arr[1]->valueAt(position), arr[2]->valueAt(position), arr[3]->valueAt(position));
      }
    }

    // General tuple case
    Tuple t;
    t.reserve(b.size());

    for(const auto& e : b)
    {
      if(e)
        t.push_back(eggs::variants::apply(*this, e));
      else
        return {};
    }

    return std::move(t);
  }
};

ossia::value
automation::computeValue(
    double position,
    const ossia::behavior& drive)
{
  if(drive)
    return eggs::variants::apply(computeValue_visitor{position}, drive);
  return {};
}

}
