#include <ossia/editor/expression/expression_composition.hpp>
#include <algorithm>

using namespace OSSIA;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionComposition final : public ExpressionComposition
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<Expression>  mFirstExpression;
  Operator                mOperator;
  std::shared_ptr<Expression>  mSecondExpression;

  Expression::iterator    mFirstResultCallbackIndex;
  Expression::iterator    mSecondResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionComposition(std::shared_ptr<Expression> expr1, Operator op, std::shared_ptr<Expression> expr2);

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
# pragma mark Accessors

  const std::shared_ptr<Expression> & getFirstOperand() const override;

  ExpressionComposition::Operator getOperator() const override;

  const std::shared_ptr<Expression> & getSecondOperand() const override;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

# pragma mark -
# pragma mark Implementation Specific

  bool do_evaluation(bool first, bool second) const;

  void firstResultCallback(bool first_result);

  void secondResultCallback(bool second_result);

};
}
