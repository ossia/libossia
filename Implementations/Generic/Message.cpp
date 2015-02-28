#include "Editor/Message.h"

namespace Generic {

class Message : public OSSIA::Message {

public:

  //todo dummy
  std::shared_ptr<OSSIA::Address> addr;
  OSSIA::AddressValue val;

  // Constructors, destructor, cloning
  Message(std::shared_ptr<OSSIA::Address> addr, OSSIA::AddressValue val) {}
  Message(const Message * other) {}
  virtual ~Message() {}
  virtual std::shared_ptr<OSSIA::Message> clone() const override {
    return std::shared_ptr<OSSIA::Message>(new Message(this));
  }

  // Lecture
  virtual void launch() const override {}

  // Accessors
  virtual std::shared_ptr<OSSIA::Address> & getAddress() const override {
    return addr; //todo cannot return non-const member in const method
  }
  virtual OSSIA::AddressValue getValue() const override {return val;}

};

}

std::shared_ptr<OSSIA::Message> OSSIA::Message::create(
    std::shared_ptr<Address> addr,
    OSSIA::AddressValue val) {
  return std::shared_ptr<OSSIA::Message>(new Generic::Message(addr, val));
}
