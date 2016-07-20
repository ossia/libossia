#include <Editor/ExpressionPulse.h>
#include <Network/Address.h>
#include <Editor/Value/Destination.h>
#include <algorithm>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaExpressionPulse final : public ExpressionPulse
{

private:

# pragma mark -
# pragma mark Implementation specific

  Destination         mDestination;
  mutable bool        mResult;

  Address::iterator   mDestinationCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionPulse(const Destination& destination);

  JamomaExpressionPulse(const JamomaExpressionPulse& other);

  shared_ptr<ExpressionPulse> clone() const override;

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

  Expression::iterator addCallback(ResultCallback callback) override;

  void removeCallback(Expression::iterator callback) override;

# pragma mark -
# pragma mark Accessors

  const Destination& getDestination() const override;

private:

# pragma mark -
# pragma mark Implementation Specific

  void destinationCallback(const SafeValue& value);
};
