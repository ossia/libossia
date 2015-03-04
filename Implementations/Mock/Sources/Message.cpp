#include <memory>

#include "Editor/Message.h"

using namespace OSSIA;

class MockMessage : public Message {

public:

  //todo dummy
  std::shared_ptr<Address> addr;
  AddressValue val;

  // Constructors, destructor, cloning
  MockMessage(std::shared_ptr<Address> addr, AddressValue val) {}
  MockMessage(const Message * other) {}
  virtual ~MockMessage() {}
  virtual std::shared_ptr<Message> clone() const override {
    return std::shared_ptr<Message>(new MockMessage(this));
  }

  // Lecture
  virtual void launch() const override {}

  // Accessors
  virtual std::shared_ptr<Address> & getAddress() const override {
    return addr; //todo cannot return non-const member in const method
  }
  virtual AddressValue getValue() const override {return val;}

};

std::shared_ptr<Message> Message::create(
    std::shared_ptr<Address> addr,
    AddressValue val) {
  return std::shared_ptr<Message>(new MockMessage(addr, val));
}
