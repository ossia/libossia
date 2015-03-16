#include "Editor/TimeProcessList.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeProcessList : public TimeProcessList
{
  
private:
  // Implementation Specific
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

  // Std container
  virtual iterator begin() override
  {
    return iterator();
  }
  
  virtual iterator end() override
  {
    return iterator();
  }
  
  virtual const_iterator cbegin() const override
  {
    return iterator();
  }
  
  virtual const_iterator cend() const override
  {
    return iterator();
  }
  
  virtual size_type size() const override
  {
    return size_type();
  }
  
  virtual bool empty() const override
  {
    return true;
  }
  
  virtual shared_ptr<TimeProcess> & front() override
  {
    return element;
  }
  
  virtual const shared_ptr<TimeProcess> & front() const override
  {
    return element;
  }
  
  virtual shared_ptr<TimeProcess> & back() override
  {
    return element;
  }
  
  virtual const shared_ptr<TimeProcess> & back() const override
  {
    return element;
  }
  
  virtual iterator insert(const_iterator where, shared_ptr<TimeProcess> what) override
  {
    return iterator();
  }
  
  virtual iterator erase(const_iterator which) override
  {
    return iterator();
  }
  
  virtual iterator erase(const_iterator first, const_iterator last) override
  {
    return iterator();
  }

};
