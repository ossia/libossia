#include <ossia/editor/expression/expression_composition.hpp>
#include <algorithm>

using namespace ossia;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionComposition final : public expression_composition
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<expression_base>  mFirstExpression;
  Operator                mOperator;
  std::shared_ptr<expression_base>  mSecondExpression;

  expression_base::iterator    mFirstResultCallbackIndex;
  expression_base::iterator    mSecondResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionComposition(std::shared_ptr<expression_base> expr1, Operator op, std::shared_ptr<expression_base> expr2);

  ~JamomaExpressionComposition();

# pragma mark -
# pragma mark Execution

  bool evaluate() const override;

  void update() const override;

# pragma mark -
# pragma mark Operator

  bool operator== (const expression_base& expression_base) const override;

  bool operator!= (const expression_base& expression_base) const override;

# pragma mark -
# pragma mark Accessors

  const std::shared_ptr<expression_base> & getFirstOperand() const override;

  expression_composition::Operator getOperator() const override;

  const std::shared_ptr<expression_base> & getSecondOperand() const override;

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
