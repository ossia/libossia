#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve.hpp>
#include <iostream>

namespace ossia
{

automation::automation(
    Destination address, const ossia::value& drive)
    : mDrivenAddress(address)
    , mDrive(drive)
    , mLastMessage{address, ossia::value{}}
{
}

automation::automation(
    Destination address, ossia::value&& drive)
    : mDrivenAddress(address)
    , mDrive(std::move(drive))
    , mLastMessage{address, ossia::value{}}
{
}

automation::~automation() = default;

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
  // todo shouldn't it become mLastMessage ??
  return ossia::message{
      mDrivenAddress, computeValue(offset / par.getDurationNominal(), mUnit, mDrive)};
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
      mLastMessage.value = computeValue(par.getDate() / par.getDurationNominal(), mUnit, mDrive);
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
  if (auto b = mDrive.try_get<ossia::Behavior>())
  {
    if (auto& curve = b->value)
    {
      curve->reset();
    }
  }
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

const ossia::value& automation::getDriving() const
{
  return mDrive;
}

struct computeValue_visitor
{
  double position;
  ossia::unit_t unit;

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

  ossia::value operator()(const ossia::Behavior& b) const
  {
    auto base_curve = b.value.get();
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

  ossia::value operator()(const ossia::Tuple& t) const
  {
    const auto n = t.value.size();
    std::array<curve<double, float>*, 4> arr{nullptr, nullptr, nullptr, nullptr};
    int arr_pos = 0;
    if(n >= 2 &&
       n <= 4 &&
       all_of(
         t.value,
         [&] (const auto& v) {
         if(v.getType() != ossia::val_type::BEHAVIOR)
           return false;

         auto c = v.template try_get<ossia::Behavior>()->value.get();
         if(!c)
           return false;

         auto t = c->getType();
         if(t.first == ossia::curve_segment_type::DOUBLE && t.second == ossia::curve_segment_type::FLOAT)
         {
           arr[arr_pos] = static_cast<ossia::curve<double,float>*>(c);
           arr_pos++;
           return true;
         }
         return false;
    }))
    {
      // VecNf case.
      switch(arr_pos)
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
    std::vector<ossia::value> t_value;
    t_value.reserve(n);


    for (const auto& e : t.value)
    {
      t_value.push_back(e.apply(*this));
    }

    return ossia::Tuple{std::move(t_value)};
  }

};

ossia::value
automation::computeValue(
    double position,
    ossia::unit_t u,
    const ossia::value& drive)
{
  return drive.apply(computeValue_visitor{position, u});
}
}
