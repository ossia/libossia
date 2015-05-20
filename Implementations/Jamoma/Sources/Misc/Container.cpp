#include <memory>
#include <list>

#include "Misc/Container.h"

using namespace OSSIA;
using namespace std;

template <typename T>
class JamomaContainer : public virtual Container<T>
{
  
private:
  
  // Implementation specific
  std::list<T*>   _list;
  shared_ptr<T>   _element;
  
public:
  
  typedef T value_type;
  typedef T * iterator;
  typedef const T * const_iterator;
  typedef std::size_t size_type;
  
  virtual iterator begin() override
  {
    return *_list.begin();
  }
  
  virtual iterator end() override
  {
    return *_list.end();
  }
  
  virtual const_iterator cbegin() const override
  {
    return *_list.cbegin();
  }
  
  virtual const_iterator cend() const override
  {
    return *_list.cend();
  }
  
  virtual size_type size() const override
  {
    return _list.size();
  }
  
  virtual bool empty() const override
  {
    return _list.empty();
  }
  
  virtual shared_ptr<T> & front() override
  {
    // todo : _element = std::make_shared<T>(*_list.front());
    return _element;
  }
  
  virtual const shared_ptr<T> & front() const override
  {
    // todo : _element = std::make_shared<T>(*_list.front());
    return _element;
  }
  
  virtual shared_ptr<T> & back() override
  {
    // todo : _element = std::make_shared<T>(*_list.back());
    return _element;
  }
  
  virtual const shared_ptr<T> & back() const override
  {
    // todo : _element = std::make_shared<T>(*_list.back());
    return _element;
  }
  
  virtual iterator insert(const_iterator where, shared_ptr<T> what) override
  {
    return iterator(); // todo : *_list.insert(where, what);
  }
  
  virtual iterator erase(const_iterator which) override
  {
    return iterator(); // todo : *_list.erase(which);
  }
  
  virtual iterator erase(const_iterator first, const_iterator last) override
  {
    return iterator(); // todo : *_list.erase(first, last);
  }
  
};