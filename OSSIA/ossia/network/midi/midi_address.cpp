#include <ossia/network/midi/midi.hpp>
#include <ossia/network/midi/midi_address.hpp>

namespace OSSIA
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

Address& MIDIAddress::pushValue(const Value& val)
{
  mValue = val;
  mProtocol.push(*this);
  return *this;
}

Address& MIDIAddress::pushValue()
{
  mProtocol.push(*this);
  return *this;
}

const OSSIA::Value& MIDIAddress::getValue() const
{
  return mValue;
}

OSSIA::Value MIDIAddress::cloneValue(DestinationIndex) const
{
  // TODO use the vec parameter
  return mValue;
}

Address&MIDIAddress::setValue(const Value& v)
{
  mValue = v;
  send(mValue);
  return *this;
}

Type MIDIAddress::getValueType() const
{
  return mType;
}

Address&MIDIAddress::setValueType(Type)
{
  return *this;
}

AccessMode MIDIAddress::getAccessMode() const
{
  return OSSIA::AccessMode::BI;
}

Address&MIDIAddress::setAccessMode(AccessMode)
{
  return *this;
}

const OSSIA::net::Domain&MIDIAddress::getDomain() const
{
  return mDomain;
}

Address&MIDIAddress::setDomain(const OSSIA::net::Domain&)
{
  return *this;
}

BoundingMode MIDIAddress::getBoundingMode() const
{
  return OSSIA::BoundingMode::CLIP;
}

Address&MIDIAddress::setBoundingMode(BoundingMode)
{
  return *this;
}

RepetitionFilter MIDIAddress::getRepetitionFilter() const
{
  return OSSIA::RepetitionFilter::OFF;
}

Address&MIDIAddress::setRepetitionFilter(RepetitionFilter)
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

void MIDIAddress::valueCallback(const OSSIA::Value& val)
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
