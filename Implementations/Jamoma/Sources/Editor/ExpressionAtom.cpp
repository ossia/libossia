#include "Editor/ExpressionAtom.h"
#include "Network/AddressValue.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionAtom : public ExpressionAtom
{
  
private:
  
  // Implementation specific
  shared_ptr<ExpressionValue> first_expr;
  shared_ptr<ExpressionValue> second_expr;
  
public:
# pragma mark -
# pragma mark Life cycle
  JamomaExpressionAtom(std::shared_ptr<ExpressionValue> expr1, Operator op, std::shared_ptr<ExpressionValue> expr2)
  {}
  
  JamomaExpressionAtom(const JamomaExpressionAtom * other)
  {}
  
  virtual ~JamomaExpressionAtom()
  {}
  
  virtual shared_ptr<ExpressionAtom> clone() const override
  {
    return make_shared<JamomaExpressionAtom>(this);
  }

# pragma mark -
# pragma mark Execution
  virtual bool evaluate() const override
  {
    return true;
  }

# pragma mark -
# pragma mark Accessors
  virtual const std::shared_ptr<ExpressionValue> & getFirstOperand() const override
  {
    return first_expr;
  }
  
  virtual const std::shared_ptr<ExpressionValue> & getSecondOperand() const override
  {
    return second_expr;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::EQUAL;
  }

};

shared_ptr<ExpressionAtom> ExpressionAtom::create(std::shared_ptr<ExpressionValue> expr1, Operator op, std::shared_ptr<ExpressionValue> expr2)
{
  return make_shared<JamomaExpressionAtom>(expr1, op, expr2);
}
