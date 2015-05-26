#include "Editor/TimeProcessList.h"

#include "TTScore.h"

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
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  virtual ~JamomaTimeProcessList()
  {}

};
