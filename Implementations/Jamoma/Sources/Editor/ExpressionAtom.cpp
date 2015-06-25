#include "Editor/ExpressionAtom.h"
#include "Editor/Value.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionAtom : public ExpressionAtom
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  Value* first_expr;
  Value* second_expr;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaExpressionAtom(const Value* expr1, Operator op, const Value* expr2) :
  first_expr(expr1->clone()),
  second_expr(expr2->clone())
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
  
  virtual const Value* getFirstOperand() const override
  {
    return first_expr;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::EQUAL;
  }
  
  virtual const Value* getSecondOperand() const override
  {
    return second_expr;
  }

};

shared_ptr<ExpressionAtom> ExpressionAtom::create(const Value* expr1, Operator op, const Value* expr2)
{
  return make_shared<JamomaExpressionAtom>(expr1, op, expr2);
}
