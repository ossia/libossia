#include "Editor/TimeProcessList.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeProcessList : public TimeProcessList
{
  
private:
  
  // Implementation specific
  shared_ptr<TimeProcess> element;
  
public:
  // Constructor, destructor
  JamomaTimeProcessList()
  {}
  
  virtual ~JamomaTimeProcessList()
  {}

};
