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
    Destination address, const ossia::value& drive)
  : mDrivenAddress(address)
  , mDrive(drive)
  , mLastMessage{address, ossia::value{}, {}}
{
}

automation::automation(
    Destination address, ossia::value&& drive)
  : mDrivenAddress(address)
  , mDrive(std::move(drive))
  , mLastMessage{address, ossia::value{}, {}}
{
}

automation::~automation() = default;

void automation::updateMessage(double t)
{
  ossia::net::address_base& addr = mDrivenAddress.value.get();
  auto& idx = mDrivenAddress.index;
  auto val = computeValue(t, mDrive);
  if(mLastMessage.unit)
  {
    /* Useful for debug :
    {
      auto v1 = ossia::net::get_value(mDrivenAddress);
      auto v2 = convert(v1, mUnit);
      auto v3 = merge(v2, val, idx);
      auto v4 = convert(v3, addr.getUnit());
      auto v5 = to_value(v4);

      std::cerr << to_pretty_string(v1) << "\n"
                << ossia::get_pretty_unit_text(mUnit) << "\n"
                << to_pretty_string(v2) << "\n"
                << to_pretty_string(v3) << "\n"
                << to_pretty_string(v4) << "\n"
                << to_pretty_string(v5) << "\n\n\n";
    }
    */

      // TODO This could be optimized by directly using the relevant visitors.
    /*
    mLastMessage.value =
        to_value( // Go from Unit domain to Value domain
          convert( // Convert to the resulting address unit
            merge( // Merge the automation value with the "unit" value
              convert( // Put the current value in the Unit domain
                ossia::net::get_value(mDrivenAddress),
                mUnit),
              std::move(val), // Compute the output of the automation
              idx),
          addr.getUnit()));
    */

    mLastMessage.value =
        to_value(
          merge( // Merge the automation value with the "unit" value
            convert( // Put the current value in the Unit domain
              ossia::net::get_value(mDrivenAddress),
              mLastMessage.unit),
            std::move(val), // Compute the output of the automation
            idx)
          );
  }
  else if(mDrivenAddress.index.size() == 1 && val.getType() == ossia::val_type::FLOAT)
  {
    switch(addr.getValueType())
    {
      case ossia::val_type::VEC2F:
      {
        auto cur_v = addr.cloneValue();
        auto& arr = cur_v.get<ossia::Vec2f>();
        if(idx[0] < arr.size_value)
        {
          arr.value[idx[0]] = val.get<Float>().value;
        }
        mLastMessage.value = arr;
        break;
      }
      case ossia::val_type::VEC3F:
      {
        auto cur_v = addr.cloneValue();
        auto& arr = cur_v.get<ossia::Vec3f>();
        if(idx[0] < arr.size_value)
        {
          arr.value[idx[0]] = val.get<Float>().value;
        }
        mLastMessage.value = arr;
        break;
      }
      case ossia::val_type::VEC4F:
      {
        auto cur_v = addr.cloneValue();
        auto& arr = cur_v.get<ossia::Vec4f>();
        if(idx[0] < arr.size_value)
        {
          arr.value[idx[0]] = val.get<Float>().value;
        }
        mLastMessage.value = arr;
        break;
      }
      default:
        mLastMessage.value = std::move(val);
        break;
    }
  }
  else
  {
    mLastMessage.value = std::move(val);
  }
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

void automation::setUnit(unit_t u)
{
  mLastMessage.unit = u;
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

  static boost::container::static_vector<curve<double, float>*, 4> tuple_convertible_to_vec(const ossia::Tuple& t)
  {
    const auto n = t.value.size();

    boost::container::static_vector<curve<double, float>*, 4> arr;
    bool ok = false;
    if(n >= 2 && n <= 4)
    {
      for(const ossia::value& v : t.value)
      {
        if(v.getType() != ossia::val_type::BEHAVIOR)
          return {};

        auto c = v.try_get<ossia::Behavior>()->value.get();
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

  ossia::value operator()(const ossia::Tuple& t) const
  {
    auto arr = tuple_convertible_to_vec(t);

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
    std::vector<ossia::value> t_value;
    t_value.reserve(t.value.size());


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
    const ossia::value& drive)
{
  return drive.apply(computeValue_visitor{position});
}

}
