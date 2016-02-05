#include "Editor/JamomaCurve.h"
#include "Editor/CurveSegment/JamomaCurveSegmentLinear.h"

#include <iostream> //! \todo to remove. only here for debug purpose

namespace OSSIA
{
  // explicit instantiation for double and bool
  template class Curve<double, bool>;

  template <>
  shared_ptr<Curve<double, bool>> Curve<double, bool>::create()
  {
    return make_shared<JamomaCurve<double, bool>>();
  }
  
  template <>
  Curve<double, bool>::~Curve()
  {}

  // explicit instantiation for double and int
  template class Curve<double, int>;

  template <>
  shared_ptr<Curve<double, int>> Curve<double, int>::create()
  {
    return make_shared<JamomaCurve<double, int>>();
  }
  
  template <>
  Curve<double, int>::~Curve()
  {}

  // explicit instantiation for double and float
  template class Curve<double, float>;

  template <>
  shared_ptr<Curve<double, float>> Curve<double, float>::create()
  {
    return make_shared<JamomaCurve<double, float>>();
  }
  
  template <>
  Curve<double, float>::~Curve()
  {}

  // explicit instantiation for bool and bool
  template class Curve<bool, bool>;

  template <>
  shared_ptr<Curve<bool, bool>> Curve<bool, bool>::create()
  {
    return make_shared<JamomaCurve<bool, bool>>();
  }
  
  template <>
  Curve<bool, bool>::~Curve()
  {}

  // explicit instantiation for bool and int
  template class Curve<bool, int>;

  template <>
  shared_ptr<Curve<bool, int>> Curve<bool, int>::create()
  {
    return make_shared<JamomaCurve<bool, int>>();
  }
  
  template <>
  Curve<bool, int>::~Curve()
  {}

  // explicit instantiation for bool and float
  template class Curve<bool, float>;

  template <>
  shared_ptr<Curve<bool, float>> Curve<bool, float>::create()
  {
    return make_shared<JamomaCurve<bool, float>>();
  }
  
  template <>
  Curve<bool, float>::~Curve()
  {}

  // explicit instantiation for int and bool
  template class Curve<int, bool>;

  template <>
  shared_ptr<Curve<int, bool>> Curve<int, bool>::create()
  {
    return make_shared<JamomaCurve<int, bool>>();
  }
  
  template <>
  Curve<int, bool>::~Curve()
  {}

  // explicit instantiation for int and int
  template class Curve<int, int>;

  template <>
  shared_ptr<Curve<int, int>> Curve<int, int>::create()
  {
    return make_shared<JamomaCurve<int, int>>();
  }
  
  template <>
  Curve<int, int>::~Curve()
  {}

  // explicit instantiation for int and float
  template class Curve<int, float>;

  template <>
  shared_ptr<Curve<int, float>> Curve<int, float>::create()
  {
    return make_shared<JamomaCurve<int, float>>();
  }
  
  template <>
  Curve<int, float>::~Curve()
  {}

  // explicit instantiation for float and bool
  template class Curve<float, bool>;

  template <>
  shared_ptr<Curve<float, bool>> Curve<float, bool>::create()
  {
    return make_shared<JamomaCurve<float, bool>>();
  }
  
  template <>
  Curve<float, bool>::~Curve()
  {}
  
  // explicit instantiation for float and int
  template class Curve<float, int>;

  template <>
  shared_ptr<Curve<float, int>> Curve<float, int>::create()
  {
    return make_shared<JamomaCurve<float, int>>();
  }
  
  template <>
  Curve<float, int>::~Curve()
  {}

  // explicit instantiation for float and float
  template class Curve<float, float>;

  template <>
  shared_ptr<Curve<float, float>> Curve<float, float>::create()
  {
    return make_shared<JamomaCurve<float, float>>();
  }
  
  template <>
  Curve<float, float>::~Curve()
  {}
}

# pragma mark -
# pragma mark Life cycle

template <typename X, typename Y>
JamomaCurve<X,Y>::
JamomaCurve() :
mInitialDestination(nullptr)
{
  mInitialDestinationIndex.push_back(0);
}

template <typename X, typename Y>
JamomaCurve<X,Y>::
JamomaCurve(const JamomaCurve * other)
{}

template <typename X, typename Y>
shared_ptr<Curve<X,Y>> JamomaCurve<X,Y>::
clone() const
{
  return make_shared<JamomaCurve>(this);
}

template <typename X, typename Y>
JamomaCurve<X,Y>::
~JamomaCurve()
{}

template <typename X, typename Y>
Curve<X,Y>::~Curve()
{}

# pragma mark -
# pragma mark Edition

template <typename X, typename Y>
bool JamomaCurve<X,Y>::
addPoint(shared_ptr<CurveSegment<Y>> segment, X abscissa, Y value)
{
  pair<Y,shared_ptr<CurveSegment<Y>>> p(value, segment);

  //! \todo check if there is already a point

  mPointsMap.emplace(abscissa, p);

  return true;
}

template <typename X, typename Y>
bool JamomaCurve<X,Y>::
removePoint(X abscissa)
{
  return mPointsMap.erase(abscissa) > 0;
}

# pragma mark -
# pragma mark Execution

template <typename X, typename Y>
Y JamomaCurve<X,Y>::
valueAt(X abscissa) const
{
  X lastAbscissa(-INFINITY);
  Y lastValue = getInitialValue();

  for (auto it = mPointsMap.begin(); it != mPointsMap.end(); it++)
  {
    if (abscissa > lastAbscissa &&
        abscissa <= it->first)
    {
      lastValue = it->second.second->valueAt((abscissa - lastAbscissa) / (it->first - lastAbscissa), lastValue , it->second.first);
      break;
    }
    else if (abscissa > it->first)
    {
      lastAbscissa = it->first;
      lastValue = it->second.first;
    }
    else
      break;
  }

  return lastValue;
}

# pragma mark -
# pragma mark Accessors

template <typename X, typename Y>
Y JamomaCurve<X,Y>::
getInitialValue() const
{
  if (mInitialDestination == nullptr)
  {
    return mInitialValue;
  }
  else
  {
    auto address = mInitialDestination->value->getAddress();

    if (!address)
      throw runtime_error("getting an address value using from a destination without address");

    char level = 0;
    return convertToTemplateTypeValue(address->pullValue(), &level);
  }
}

template <typename X, typename Y>
void JamomaCurve<X,Y>::
setInitialValue(Y value)
{
  mInitialValue = value;
}

template <typename X, typename Y>
const Destination* JamomaCurve<X,Y>::
getInitialDestination() const
{
  return mInitialDestination;
}

template <typename X, typename Y>
void JamomaCurve<X,Y>::
setInitialDestination(const Destination* destination)
{
  mInitialDestination = static_cast<Destination*>(destination->clone());
}

template <typename X, typename Y>
map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> JamomaCurve<X,Y>::
getPointsMap() const
{
  return mPointsMap;
}

# pragma mark -
# pragma mark Implementation specific

template <typename X, typename Y>
Y JamomaCurve<X,Y>::
convertToTemplateTypeValue(const Value * value, char* level) const
{
  switch (value->getType())
  {
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(value);
      return b->value;
    }

    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(value);
      return i->value;
    }

    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(value);
      return f->value;
    }

    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(value);
      return c->value;
    }

    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(value);

      char index = mInitialDestination->index[*level];
      (*level)++;

      return convertToTemplateTypeValue(t->value[index], level);
    }

    default :
    {
      throw runtime_error("converting none numerical value");
    }
  }
}
