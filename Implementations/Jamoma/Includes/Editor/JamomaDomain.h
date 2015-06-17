#include "Editor/Domain.h"
#include "Editor/Value.h"

using namespace OSSIA;
using namespace std;

class JamomaDomain : public Domain
{
  
private:
  
  // Implementation specific
  Value * mMin;
  Value * mMax;
  std::vector<Value*> mValues;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaDomain(Value * = new Impulse(),
               Value * = new Impulse(),
               std::vector<Value*> = std::vector<Value*>());
  
  ~JamomaDomain();

# pragma mark -
# pragma mark Accessors
  
  Value * getMin() const;
  
  void setMin(Value * min);
  
  Value * getMax() const;

  void setMax(Value * max);
  
  std::vector<Value*> getValues() const;
  
  void setValues(std::vector<Value*> values);
  
};

