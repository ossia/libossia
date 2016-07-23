#include "Editor/ExpressionComposition.h"
#include <algorithm>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

namespace impl
{
class JamomaExpressionComposition final : public ExpressionComposition
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Expression>  mFirstExpression;
  Operator                mOperator;
  shared_ptr<Expression>  mSecondExpression;

  Expression::iterator    mFirstResultCallbackIndex;
  Expression::iterator    mSecondResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionComposition(shared_ptr<Expression> expr1, Operator op, shared_ptr<Expression> expr2);

  JamomaExpressionComposition(const JamomaExpressionComposition * other);

  shared_ptr<ExpressionComposition> clone() const override;

  ~JamomaExpressionComposition();

# pragma mark -
# pragma mark Execution

  bool evaluate() const override;

  void update() const override;

# pragma mark -
# pragma mark Operator

  bool operator== (const Expression& expression) const override;

  bool operator!= (const Expression& expression) const override;

# pragma mark -
# pragma mark Callback Container

  Expression::iterator addCallback(ResultCallback callback) override;

  void removeCallback(Expression::iterator callback) override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<Expression> & getFirstOperand() const override;

  ExpressionComposition::Operator getOperator() const override;

  const shared_ptr<Expression> & getSecondOperand() const override;

private:

# pragma mark -
# pragma mark Implementation Specific

  bool do_evaluation(bool first, bool second) const;

  void firstResultCallback(bool first_result);

  void secondResultCallback(bool second_result);

};
}
