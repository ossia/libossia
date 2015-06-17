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
  vector<Value*> mValues;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaDomain(Value * = new Impulse(),
               Value * = new Impulse(),
               vector<Value*> = vector<Value*>());
  
  ~JamomaDomain();

# pragma mark -
# pragma mark Accessors
  
  Value * getMin() const;
  
  void setMin(Value * min);
  
  Value * getMax() const;

  void setMax(Value * max);
  
  vector<Value*> getValues() const;
  
  void setValues(vector<Value*> values);
  
};

