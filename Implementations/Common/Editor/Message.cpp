#include "Editor/JamomaMessage.h"

namespace OSSIA
{
  shared_ptr<Message> Message::create(shared_ptr<Address> a, const Value& v)
  {
    return make_shared<JamomaMessage>(a, v);
  }
}

# pragma mark -
# pragma mark Life cycle

JamomaMessage::JamomaMessage(shared_ptr<Address> a, const Value& v) :
address(a),
value(v)
{}

JamomaMessage::JamomaMessage(const JamomaMessage& other)
{}

shared_ptr<Message> JamomaMessage::clone() const
{
  return make_shared<JamomaMessage>(*this);
}

JamomaMessage::~JamomaMessage()
{}

Message::~Message()
{}

# pragma mark -
# pragma mark Execution

void JamomaMessage::launch() const
{
  address->pushValue(value);
}

# pragma mark -
# pragma mark Accessors

StateElement::Type JamomaMessage::getType() const
{
    return StateElement::Type::MESSAGE;
}

const shared_ptr<Address> & JamomaMessage::getAddress() const
{
  return address;
}

const Value& JamomaMessage::getValue() const
{
  return value;
}
