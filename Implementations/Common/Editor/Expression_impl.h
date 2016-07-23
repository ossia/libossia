#include "Editor/ExpressionAtom.h"
#include <Editor/Value/Value.h>

using namespace OSSIA;
using namespace std;

namespace impl
{
class JamomaExpression : public Expression
{

private:

# pragma mark -
# pragma mark Implementation specific

  bool m_result;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpression(bool result);

  ~JamomaExpression();

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

  //! \todo shouldn't they stay unimplemented ?
  Expression::iterator addCallback(ResultCallback) override;

  void removeCallback(Expression::iterator callback) override;
};
}
