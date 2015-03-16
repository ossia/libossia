#include "Editor/ExpressionComposition.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionComposition : public ExpressionComposition
{
  
private:
  // Implementation Specific
  shared_ptr<Expression> exp;

public:
  // Constructors, destructor, cloning
  JamomaExpressionComposition(shared_ptr<ExpressionComposition> exp1, Operator op, shared_ptr<ExpressionComposition> exp2)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaExpressionComposition(const JamomaExpressionComposition * other)
  {}
  
  virtual ~JamomaExpressionComposition()
  {}
  
  virtual shared_ptr<ExpressionComposition> clone() const override
  {
    return shared_ptr<ExpressionComposition>(new JamomaExpressionComposition(this));
  }

  // Lecture
  virtual bool evaluate() const override
  {
    return true;
  }

  // Accessors
  virtual const shared_ptr<Expression> & getFirstOperand() const override
  {
    return exp;
  }
  
  virtual const shared_ptr<Expression> & getSecondOperand() const
  {
    return exp;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::NOT;
  }

};

shared_ptr<ExpressionComposition> ExpressionComposition::create(std::shared_ptr<Address> addr, Operator op, AddressValue val)
{
  return shared_ptr<ExpressionComposition>(new JamomaExpressionComposition(addr, op, val));
}
