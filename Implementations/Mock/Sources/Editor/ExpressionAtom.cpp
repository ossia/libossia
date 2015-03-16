#include "Editor/ExpressionAtom.h"
#include "Network/AddressValue.h"
using namespace OSSIA;
using namespace std;

class MockExpressionAtom : public ExpressionAtom {

public:

  // Dummy members
  shared_ptr<Address> addr;

  // Constructors, destructor, cloning
  MockExpressionAtom(std::shared_ptr<Address> addr,
			 Operator op,
			 AddressValue val) {}
  MockExpressionAtom(const MockExpressionAtom * other) {}
  virtual ~MockExpressionAtom() {}
  virtual shared_ptr<ExpressionAtom> clone() const override {
	return shared_ptr<ExpressionAtom>(new MockExpressionAtom(this));
  }

  // Lecture
  virtual bool evaluate() const override {return true;}

  // Accessors
  virtual const shared_ptr<Address> & getAddress() const override {
	return addr;
  }
  virtual Operator getOperator() const override {return Operator::IMPULSE;}
  virtual AddressValue getOperand() const override {return AddressValue();}

};

shared_ptr<ExpressionAtom> ExpressionAtom::create(
	std::shared_ptr<Address> addr,
	Operator op,
	AddressValue val) {
  return shared_ptr<ExpressionAtom>(new MockExpressionAtom(addr, op, val));
}
