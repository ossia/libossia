#include <ossia/network/midi/midi.hpp>
#include <ossia/network/midi/midi_address.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
midi_address::midi_address(address_info info, node_base& parent)
    : mInfo{info}
    , mParent{parent}
    , mProtocol{dynamic_cast<midi_protocol&>(parent.getDevice().getProtocol())}
    , mDomain{mInfo.defaultDomain()}
    , mType{mInfo.matchingType()}
    , mValue{mInfo.defaultValue(64)}
{
}

const address_info& midi_address::info() const
{
  return mInfo;
}

node_base& midi_address::getNode() const
{
  return mParent;
}

void midi_address::pullValue()
{
  mProtocol.pull(*this);
}

address_base& midi_address::pushValue(const value& val)
{
  mValue = val;
  mProtocol.push(*this);
  return *this;
}

address_base& midi_address::pushValue()
{
  mProtocol.push(*this);
  return *this;
}

const ossia::value& midi_address::getValue() const
{
  return mValue;
}

ossia::value midi_address::cloneValue() const
{
  return mValue;
}

address_base& midi_address::setValue(const value& v)
{
  mValue = v;
  send(mValue);
  return *this;
}

val_type midi_address::getValueType() const
{
  return mType;
}

address_base& midi_address::setValueType(val_type)
{
  return *this;
}

access_mode midi_address::getAccessMode() const
{
  return ossia::access_mode::BI;
}

address_base& midi_address::setAccessMode(access_mode)
{
  return *this;
}

const ossia::domain& midi_address::getDomain() const
{
  return mDomain;
}

address_base& midi_address::setDomain(const ossia::domain&)
{
  return *this;
}

bounding_mode midi_address::getBoundingMode() const
{
  return ossia::bounding_mode::CLIP;
}

address_base& midi_address::setBoundingMode(bounding_mode)
{
  return *this;
}

repetition_filter midi_address::getRepetitionFilter() const
{
  return ossia::repetition_filter::OFF;
}

address_base& midi_address::setRepetitionFilter(repetition_filter)
{
  return *this;
}

void midi_address::onFirstCallbackAdded()
{
  mProtocol.observe(*this, true);
}

void midi_address::onRemovingLastCallback()
{
  mProtocol.observe(*this, false);
}

void midi_address::valueCallback(const ossia::value& val)
{
  this->setValue(val);
}

}
}
}
