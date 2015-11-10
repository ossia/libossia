#include "Editor/Message.h"
#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class JamomaMessage final : public Message
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Address> address;
  Value * value;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMessage(shared_ptr<Address> a, const Value * v);

  JamomaMessage(const JamomaMessage * other);

  shared_ptr<Message> clone() const override;

  ~JamomaMessage();

# pragma mark -
# pragma mark Execution

  void launch() const override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<Address> & getAddress() const override;

  Value * getValue() const override;
};
