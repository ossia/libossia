#include "Editor/Message.h"
#include "Network/AddressValue.h"

using namespace OSSIA;
using namespace std;


class MockMessage : public Message {

public:

  // Dummy members
  shared_ptr<Address> addr;
  AddressValue * val;

  // Constructors, destructor, cloning
  MockMessage(shared_ptr<Address> addr, AddressValue val) {}
  MockMessage(const MockMessage * other) {}
  virtual ~MockMessage() {}
  virtual shared_ptr<Message> clone() const override {
    return shared_ptr<Message>(new MockMessage(this));
  }

  // Lecture
  virtual void launch() const override {}

  // Accessors
  virtual shared_ptr<Address> & getAddress() const override {
    auto ret = addr;
    return ret; //TODO cannot return non-const member in const method
  }
  virtual AddressValue * getValue() const override {return val;}

};

shared_ptr<Message> Message::create(
    shared_ptr<Address> addr,
    AddressValue * val) {
  return shared_ptr<Message>(new MockMessage(addr, *val));
}
