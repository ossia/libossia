/*!
 * \file Container.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "Misc/Iterators.h"

namespace OSSIA {

template <typename T>
class Container {



public:

  typedef T value_type;
  typedef genericIterator iterator;
  typedef genericConst_iterator const_iterator;
  typedef genericSize_type size_type;
  virtual ~Container() = default;

  virtual iterator begin() = 0;
  virtual iterator end() = 0;
  virtual const_iterator cbegin() const = 0;
  virtual const_iterator cend() const = 0;
  virtual size_type size() const = 0;
  virtual bool empty() const = 0;
  virtual std::shared_ptr<T> & front() = 0;
  virtual const std::shared_ptr<T> & front() const = 0;
  virtual std::shared_ptr<T> & back() = 0;
  virtual const std::shared_ptr<T> & back() const = 0;
  virtual iterator insert(const_iterator, std::shared_ptr<T>) = 0;
  virtual iterator erase(const_iterator) = 0;
  virtual iterator erase(const_iterator first, const_iterator last) = 0;

};

}



#endif /* CONTAINER_H_ */
