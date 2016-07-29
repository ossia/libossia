#include <ossia/editor/expression/expression_pulse.hpp>
#include <ossia/editor/value/destination.hpp>
#include <ossia/network/base/address.hpp>
#include <algorithm>

using namespace ossia;

using namespace std::placeholders;

namespace impl
{
class JamomaExpressionPulse final : public expression_pulse
{

private:

# pragma mark -
# pragma mark Implementation specific

  Destination         mDestination;
  mutable bool        mResult;

  net::address::callback_index   mDestinationCallbackIndex;

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

  bool operator== (const expression_base& expression_base) const override;

  bool operator!= (const expression_base& expression_base) const override;

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

  void destinationCallback(const value& value);
};
}
