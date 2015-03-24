#include <memory>

#include "Misc/Container.h"

using namespace OSSIA;
using namespace std;

template <typename T>
class MockContainer : public virtual Container<T> {

public:

  typedef T value_type;
  typedef T * iterator;
  typedef const T * const_iterator;
  typedef std::size_t size_type;

  // Dummy element
  shared_ptr<T> element;

  virtual iterator begin() override {return iterator();}
  virtual iterator end() override {return iterator();}
  virtual const_iterator cbegin() const override {return iterator();}
  virtual const_iterator cend() const override {return iterator();}
  virtual size_type size() const override {return size_type();}
  virtual bool empty() const override {return true;}
  virtual shared_ptr<T> & front() override {
        return element;
  }
  virtual const shared_ptr<T> & front() const override {
        return element;
  }
  virtual shared_ptr<T> & back() override {
        return element;
  }
  virtual const shared_ptr<T> & back() const override {
        return element;
  }
  virtual iterator insert(const_iterator where,
                          shared_ptr<T> what) override {
        return iterator();
  }
  virtual iterator erase(const_iterator which) override {return iterator();}
  virtual iterator erase(const_iterator first,
                         const_iterator last) override {return iterator();}

};
