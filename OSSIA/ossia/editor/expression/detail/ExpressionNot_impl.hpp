#include <ossia/editor/expression/expression_not.hpp>
#include <algorithm>

using namespace ossia;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionNot final : public expression_not
{

private:

# pragma mark -
# pragma mark Implementation specific

  std::shared_ptr<expression_base>  mExpression;

  expression_base::iterator    mResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionNot(std::shared_ptr<expression_base> expr);

  ~JamomaExpressionNot();

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

  const std::shared_ptr<expression_base> & getExpression() const override;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

# pragma mark -
# pragma mark Implementation Specific

  void resultCallback(bool result);
};
}
