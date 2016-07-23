#include "Message_impl.h"

namespace impl
{

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

}
