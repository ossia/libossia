#include "Editor/Message.h"
#include "Network/Address.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaMessage : public Message
{
  
private:
  
  // Implementation specific
  shared_ptr<Address> addr;
  AddressValue val;

public:
  // Constructors, destructor, cloning
  JamomaMessage(shared_ptr<Address> addr, AddressValue val)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaMessage(const JamomaMessage * other)
  {}
  
  virtual ~JamomaMessage()
  {}
  
  virtual shared_ptr<Message> clone() const override
  {
    return shared_ptr<Message>(new JamomaMessage(this));
  }

  // Lecture
  virtual void launch() const override
  {}

  // Accessors
  virtual const shared_ptr<Address> & getAddress() const override
  {
    return addr; //todo cannot return non-const member in const method
  }
  
  virtual AddressValue getValue() const override
  {
    return val;
  }

};

shared_ptr<Message> Message::create(shared_ptr<Address> addr, AddressValue val)
{
  return shared_ptr<Message>(new JamomaMessage(addr, val));
}
