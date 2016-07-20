#include "Editor/Message.h"
#include "Network/Address.h"

#include "JamomaStateElement.h"

using namespace OSSIA;
using namespace std;

class JamomaMessage final : public Message, public JamomaStateElement
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Address> address;
  SafeValue value;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaMessage(shared_ptr<Address> a, const SafeValue& v);

  JamomaMessage(const JamomaMessage& other);

  shared_ptr<Message> clone() const override;

  ~JamomaMessage();

# pragma mark -
# pragma mark Execution

  void launch() const override;

# pragma mark -
# pragma mark Accessors

  StateElement::Type getType() const override final;

  const shared_ptr<Address> & getAddress() const override;

  const SafeValue& getValue() const override;
};
