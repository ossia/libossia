#include <ossia/network/midi/midi.hpp>
#include <ossia/network/midi/midi_address.hpp>

namespace ossia
{
namespace net
{

MIDIAddress::MIDIAddress(MIDIAddressInfo info, Node& parent):
  mInfo{info},
  mParent{parent},
  mProtocol{dynamic_cast<MIDI&>(parent.getDevice().getProtocol())},
  mDomain{mInfo.defaultDomain()},
  mType{mInfo.matchingType()},
  mValue{mInfo.defaultValue(64)},
  mAddress{mInfo.address()}
{
}

const MIDIAddressInfo& MIDIAddress::info() const
{
  return mInfo;
}

const Node& MIDIAddress::getNode() const
{
  return mParent;
}

void MIDIAddress::pullValue()
{
  mProtocol.pull(*this);
}

address& MIDIAddress::pushValue(const value& val)
{
  mValue = val;
  mProtocol.push(*this);
  return *this;
}

address& MIDIAddress::pushValue()
{
  mProtocol.push(*this);
  return *this;
}

const ossia::value& MIDIAddress::getValue() const
{
  return mValue;
}

ossia::value MIDIAddress::cloneValue(destination_index) const
{
  // TODO use the vec parameter
  return mValue;
}

address&MIDIAddress::setValue(const value& v)
{
  mValue = v;
  send(mValue);
  return *this;
}

Type MIDIAddress::getValueType() const
{
  return mType;
}

address&MIDIAddress::setValueType(Type)
{
  return *this;
}

AccessMode MIDIAddress::getAccessMode() const
{
  return ossia::AccessMode::BI;
}

address&MIDIAddress::setAccessMode(AccessMode)
{
  return *this;
}

const ossia::net::Domain&MIDIAddress::getDomain() const
{
  return mDomain;
}

address&MIDIAddress::setDomain(const ossia::net::Domain&)
{
  return *this;
}

BoundingMode MIDIAddress::getBoundingMode() const
{
  return ossia::BoundingMode::CLIP;
}

address&MIDIAddress::setBoundingMode(BoundingMode)
{
  return *this;
}

RepetitionFilter MIDIAddress::getRepetitionFilter() const
{
  return ossia::RepetitionFilter::OFF;
}

address&MIDIAddress::setRepetitionFilter(RepetitionFilter)
{
  return *this;
}

void MIDIAddress::onFirstCallbackAdded()
{
  mProtocol.observe(*this, true);
}

void MIDIAddress::onRemovingLastCallback()
{
  mProtocol.observe(*this, false);
}

void MIDIAddress::valueCallback(const ossia::value& val)
{
  this->setValue(val);
}

const std::string& MIDIAddress::getTextualAddress() const
{
  // TODO !!
  assert(false);
  return {};
}


}
}
