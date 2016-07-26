#include <ossia/editor/expression/expression_not.hpp>
#include <algorithm>

using namespace OSSIA;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionNot final : public ExpressionNot
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<Expression>  mExpression;

  Expression::iterator    mResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionNot(std::shared_ptr<Expression> expr);

  JamomaExpressionNot(const JamomaExpressionNot * other);

  std::shared_ptr<ExpressionNot> clone() const override;

  ~JamomaExpressionNot();

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

  const std::shared_ptr<Expression> & getExpression() const override;

private:

# pragma mark -
# pragma mark Implementation Specific

  void resultCallback(bool result);
};
}
