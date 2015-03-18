#include "Editor/ExpressionAtom.h"
#include "Network/AddressValue.h"
using namespace OSSIA;
using namespace std;

class MockExpressionAtom : public ExpressionAtom {

public:

  // Dummy members
  shared_ptr<ExpressionValue> val;

  // Constructors, destructor, cloning
  MockExpressionAtom(std::shared_ptr<ExpressionValue> val1,
                     Operator op,
                     std::shared_ptr<ExpressionValue> val2) {}
  MockExpressionAtom(const MockExpressionAtom * other) {}
  virtual ~MockExpressionAtom() {}
  virtual shared_ptr<ExpressionAtom> clone() const override {
	return shared_ptr<ExpressionAtom>(new MockExpressionAtom(this));
  }

  // Lecture
  virtual bool evaluate() const override {return true;}

  // Accessors
  virtual const shared_ptr<ExpressionValue> & getFirstOperand() const override {
        return val;
  }
  virtual const shared_ptr<ExpressionValue> & getSecondOperand() const override {
        return val;
  }
  virtual Operator getOperator() const override {return Operator::EQUAL;}

};

shared_ptr<ExpressionAtom> ExpressionAtom::create(
    std::shared_ptr<ExpressionValue> val1,
    Operator op,
    std::shared_ptr<ExpressionValue> val2) {
  return shared_ptr<ExpressionAtom>(new MockExpressionAtom(val1, op, val2));
}
