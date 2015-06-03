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
  
  // Life cycle
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
    return shared_ptr<Message>(new JamomaMessage(this));
  }

  // Execution
  virtual void launch() const override
  {
    address->sendValue(value);
  }

  // Accessors
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
  return shared_ptr<Message>(new JamomaMessage(a, v));
}
