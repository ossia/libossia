#include "Network/Node.h"
#include "../Misc/Container.cpp"

using namespace OSSIA;
using namespace std;

class JamomaNode : public virtual Node, public JamomaContainer<Node>
{
  
private:
  
  // Implementation specific
  shared_ptr<Address> addr;
  JamomaNode * parent;

public:

  // Constructor, destructor
  JamomaNode(string) {}
  ~JamomaNode() {}

  // Navigation
  virtual Node & getParent() const override {return *parent;}

  // Accessors
  virtual string getName() const override {return "";}
  virtual const shared_ptr<Address> & getAddress() const override {return addr;}
  virtual bool removeAddress() override {return false;}

  // Address Factory
  virtual shared_ptr<Address> createAddress(AddressValue::Type) override {
    return addr;
  }

  // Child Node Factory
  virtual iterator emplace(const_iterator, string) override {return iterator();}

};
