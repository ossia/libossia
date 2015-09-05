#include "Editor/ExpressionAtom.h"
#include "Editor/Value.h"

using namespace OSSIA;
using namespace std;

class JamomaExpression : public Expression
{

private:

# pragma mark -
# pragma mark Implementation specific

  bool m_result;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpression(bool result) :
  m_result(result)
  {}

  ~JamomaExpression()
  {}

# pragma mark -
# pragma mark Execution

  bool evaluate() const override
  {
    return m_result;
  }

# pragma mark -
# pragma mark Callback Container

  //! \todo shouldn't they stay unimplemented ?
  Expression::iterator addCallback(ResultCallback) override
  {
      return callbacks().end();
  }

  void removeCallback(Expression::iterator callback) override
  {}
};

namespace OSSIA
{
  shared_ptr<Expression> Expression::create(bool result)
  {
    return make_shared<JamomaExpression>(result);
  }
}
