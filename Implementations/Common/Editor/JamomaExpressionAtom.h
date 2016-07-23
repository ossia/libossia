#include "Editor/ExpressionAtom.h"
#include <Editor/Value/Value.h>
#include "Network/Address.h"
#include "Network/Node.h"

#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaExpressionAtom : public ExpressionAtom
{

private:

# pragma mark -
# pragma mark Implementation specific

  SafeValue    mFirstValue;
  Operator  mOperator;
  SafeValue    mSecondValue;

  Address::iterator mFirstValueCallbackIndex;
  Address::iterator mSecondValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionAtom(const SafeValue& value1, Operator op, const SafeValue& value2);

  JamomaExpressionAtom(const JamomaExpressionAtom& other);

  shared_ptr<ExpressionAtom> clone() const override;

  ~JamomaExpressionAtom();

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

  const SafeValue& getFirstOperand() const override;

  ExpressionAtom::Operator getOperator() const override;

  const SafeValue& getSecondOperand() const override;

private:

# pragma mark -
# pragma mark Implementation Specific

  bool do_evaluation(const SafeValue& first, const SafeValue& second) const;

  void firstValueCallback(const SafeValue& value);

  void secondValueCallback(const SafeValue& value);

};
