#include "Editor/Message.h"
#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class JamomaMessage : public Message
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Address> address;
  Value * value;

public:
  
# pragma mark -
# pragma mark Life cycle
  JamomaMessage(shared_ptr<Address> a, const Value * v) :
  address(a),
  value(v->clone())
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
  
  virtual Value * getValue() const override
  {
    return value;
  }
};

namespace OSSIA
{
  shared_ptr<Message> Message::create(shared_ptr<Address> a, const Value * v)
  {
    return make_shared<JamomaMessage>(a, v);
  }
}
