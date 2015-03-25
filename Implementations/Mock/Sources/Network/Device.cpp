#include "Network/Device.h"
#include "Node.cpp"

using namespace OSSIA;
using namespace std;

class MockDevice : public Device, public MockNode {

public:

  // Constructor, destructor
  MockDevice(Protocol &, string name = "") : MockNode(name) {}
  ~MockDevice() {}

  // Network
  virtual bool updateNamespace() override {return false;}

};

shared_ptr<Device> Device::create(Protocol & p, string name) {
  return shared_ptr<Device>(new MockDevice(p, name));
}

shared_ptr<Node> declareMinuitInternDevice(string, int) {
  return shared_ptr<Node>(new MockNode(""));
}

shared_ptr<Node> declareOSCInternDevice(int, int) {
  return shared_ptr<Node>(new MockNode(""));
}
