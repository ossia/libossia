#include "Editor/Message.h"
#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class JamomaMessage : public Message
{
  
private:
  
  // Implementation specific
  shared_ptr<Address> address;
  AddressValue * value;

public:
  
# pragma mark -
# pragma mark Life cycle
  JamomaMessage(shared_ptr<Address> a, AddressValue * v) :
  address(a),
  value(v)
  {}
  
  JamomaMessage(const JamomaMessage * other)
  {}
  
  virtual ~JamomaMessage()
  {}
  
  virtual shared_ptr<Message> clone() const override
  {
    return make_shared<JamomaMessage>(this);
  }

# pragma mark -
# pragma mark Execution
  virtual void launch() const override
  {
    address->sendValue(value);
  }

# pragma mark -
# pragma mark Accessors
  virtual const shared_ptr<Address> & getAddress() const override
  {
    return address;
  }
  
  virtual AddressValue * getValue() const override
  {
    return value;
  }
};

shared_ptr<Message> Message::create(shared_ptr<Address> a, AddressValue * v)
{
  return make_shared<JamomaMessage>(a, v);
}
