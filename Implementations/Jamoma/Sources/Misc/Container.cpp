#include <memory>
#include <list>
#include <algorithm>

#include "Misc/Container.h"

using namespace OSSIA;
using namespace std;

template <typename T>
class JamomaContainer : public virtual Container<T>
{
  
private:
  
  // Implementation specific
  std::list<shared_ptr<T>> _list;
  
public:
  
  typedef T value_type;
  typedef shared_ptr<T> iterator;
  typedef const shared_ptr<T> const_iterator;
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
    return _list.front();
  }
  
  virtual const shared_ptr<T> & front() const override
  {
    return _list.front();
  }
  
  virtual shared_ptr<T> & back() override
  {
    return _list.back();
  }
  
  virtual const shared_ptr<T> & back() const override
  {
    return _list.back();
  }
  
  virtual iterator insert(const_iterator where, const shared_ptr<T> & what) override
  {
    auto it = std::find(_list.cbegin(), _list.cend(), where);
    return *_list.insert(it, what);
  }
  
  virtual iterator erase(const_iterator which) override
  {
    auto it = std::find(_list.cbegin(), _list.cend(), which);
    return *_list.erase(it);
  }
  
  virtual iterator erase(const_iterator first, const_iterator last) override
  {
    auto f_it = std::find(_list.cbegin(), _list.cend(), first);
    auto l_it = std::find(_list.cbegin(), _list.cend(), last);
    return *_list.erase(f_it, l_it);
  }
  
};
