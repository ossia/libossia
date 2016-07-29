#include <ossia/editor/expression/expression_atom.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>

#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionAtom : public ExpressionAtom
{

private:

# pragma mark -
# pragma mark Implementation specific

  Value    mFirstValue;
  Operator  mOperator;
  Value    mSecondValue;

  net::Address::callback_index mFirstValueCallbackIndex;
  net::Address::callback_index mSecondValueCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionAtom(const Value& value1, Operator op, const Value& value2);

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
# pragma mark Accessors

  const Value& getFirstOperand() const override;

  ExpressionAtom::Operator getOperator() const override;

  const Value& getSecondOperand() const override;

private:
  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

# pragma mark -
# pragma mark Implementation Specific

  bool do_evaluation(const Value& first, const Value& second) const;

  void firstValueCallback(const Value& value);

  void secondValueCallback(const Value& value);

};
}
