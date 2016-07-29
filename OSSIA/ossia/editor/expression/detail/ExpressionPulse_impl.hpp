#include <ossia/editor/expression/expression_pulse.hpp>
#include <ossia/editor/value/destination.hpp>
#include <ossia/network/base/address.hpp>
#include <algorithm>

using namespace OSSIA;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionPulse final : public ExpressionPulse
{

private:

# pragma mark -
# pragma mark Implementation specific

  Destination         mDestination;
  mutable bool        mResult;

  net::Address::callback_index   mDestinationCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionPulse(const Destination& destination);

  ~JamomaExpressionPulse();

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

  void onFirstCallbackAdded() override;
  void onRemovingLastCallback() override;

# pragma mark -
# pragma mark Accessors

  const Destination& getDestination() const override;

private:

# pragma mark -
# pragma mark Implementation Specific

  void destinationCallback(const Value& value);
};
}
