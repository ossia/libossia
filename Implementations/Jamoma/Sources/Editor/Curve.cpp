#include "Editor/JamomaCurve.h"
#include "Editor/CurveSegment/JamomaCurveSegmentLinear.h"
#include <Editor/Value/SafeValue.h>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

template <typename X, typename Y>
JamomaCurve<X,Y>::
JamomaCurve() :
mInitialPointAbscissaDestination(nullptr),
mInitialPointOrdinateDestination(nullptr)
{}

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

# pragma mark -
# pragma mark Edition

template <typename X, typename Y>
bool JamomaCurve<X,Y>::
addPoint(shared_ptr<CurveSegment<Y>> segment, X abscissa, Y value)
{
  pair<Y,shared_ptr<CurveSegment<Y>>> p(value, segment);

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
  X lastAbscissa = getInitialPointAbscissa();
  Y lastValue = getInitialPointOrdinate();

  auto end = mPointsMap.end();
  for (auto it = mPointsMap.begin(); it != end; ++it)
  {
    if (abscissa > lastAbscissa &&
        abscissa <= it->first)
    {
      lastValue = it->second.second->valueAt(
            ((double)abscissa - (double)lastAbscissa) / ((double)it->first - (double)lastAbscissa),
            lastValue,
            it->second.first);
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
template<typename T>
constexpr nullptr_t OssiaType{};
template<>
constexpr OSSIA::CurveSegmentType OssiaType<int> = OSSIA::CurveSegmentType::INT;
template<>
constexpr OSSIA::CurveSegmentType OssiaType<float> = OSSIA::CurveSegmentType::FLOAT;
template<>
constexpr OSSIA::CurveSegmentType OssiaType<double> = OSSIA::CurveSegmentType::DOUBLE;
template<>
constexpr OSSIA::CurveSegmentType OssiaType<bool> = OSSIA::CurveSegmentType::BOOL;

template<typename X, typename Y>
OSSIA::CurveType JamomaCurve<X, Y>::getType() const
{
    return std::make_pair(OssiaType<X>, OssiaType<Y>);
}

template <typename X, typename Y>
X JamomaCurve<X,Y>::
getInitialPointAbscissa() const
{
  auto address = mInitialPointAbscissaDestination.value->getAddress();

  if (!address)
    throw runtime_error("getting an address value using from an abscissa destination without address");

  address->pullValue();
  auto val = address->cloneValue();
  auto res = convertToTemplateTypeValue(val, mInitialPointAbscissaDestination.index.begin());
  return res;
}

template <typename X, typename Y>
Y JamomaCurve<X,Y>::
getInitialPointOrdinate() const
{
  auto address = mInitialPointOrdinateDestination.value->getAddress();

  if (!address)
    throw runtime_error("getting an address value using from an ordinate destination without address");

  address->pullValue();
  auto val = address->cloneValue();
  auto res = convertToTemplateTypeValue(val, mInitialPointOrdinateDestination.index.begin());
  return res;
}

template <typename X, typename Y>
void JamomaCurve<X,Y>::
setInitialPointAbscissa(X value)
{
  mInitialPointAbscissa = value;
}

template <typename X, typename Y>
void JamomaCurve<X,Y>::
setInitialPointOrdinate(Y value)
{
  mInitialPointOrdinate = value;
}

template <typename X, typename Y>
const Destination& JamomaCurve<X,Y>::
getInitialPointAbscissaDestination() const
{
  return mInitialPointAbscissaDestination;
}

template <typename X, typename Y>
const Destination& JamomaCurve<X,Y>::
getInitialPointOrdinateDestination() const
{
  return mInitialPointOrdinateDestination;
}

template <typename X, typename Y>
void JamomaCurve<X,Y>::
setInitialPointAbscissaDestination(const Destination& destination)
{
  mInitialPointAbscissaDestination = destination;
}

template <typename X, typename Y>
void JamomaCurve<X,Y>::
setInitialPointOrdinateDestination(const Destination& destination)
{
  mInitialPointOrdinateDestination = destination;
}

template <typename X, typename Y>
map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> JamomaCurve<X,Y>::
getPointsMap() const
{
  map<X, pair<Y, shared_ptr<CurveSegment<Y>>>> m;
  m.insert(mPointsMap.begin(), mPointsMap.end());
  return m;
}

# pragma mark -
# pragma mark Implementation specific

template <typename X, typename Y>
Y JamomaCurve<X,Y>::
convertToTemplateTypeValue(const SafeValue& value, vector<char>::const_iterator index) const
{
  switch (value.getType())
  {
    /*
    case Type::BOOL :
    {
      auto b = static_cast<const Bool*>(value);
      return b->value;
    }

    case Type::INT :
    {
      auto i = static_cast<const Int*>(value);
      return i->value;
    }

    case Type::FLOAT :
    {
      auto f = static_cast<const Float*>(value);
      return f->value;
    }

    case Type::CHAR :
    {
      auto c = static_cast<const Char*>(value);
      return c->value;
    }

    case Type::VEC2F :
    {
      auto t = static_cast<const Vec2f*>(value);
      return t->value[*index];
    }
    case Type::VEC3F :
    {
      auto t = static_cast<const Vec3f*>(value);
      return t->value[*index];
    }
    case Type::VEC4F :
    {
      auto t = static_cast<const Vec4f*>(value);
      return t->value[*index];
    }

    case Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(value);
      return convertToTemplateTypeValue(t->value[*index], index++);
    }

    default :
    {
      throw runtime_error("Cannot convert to a numeric type");
    }
    */
  }
}



namespace OSSIA
{
    // Curve implementation
    template<class X, class Y>
    std::shared_ptr<Curve<X, Y>> Curve<X, Y>::create()
    {
        return make_shared<JamomaCurve<X, Y>>();
    }
    template<class X, class Y>
    Curve<X, Y>::~Curve()
    {
    }

    // CurveSegment implementation
    template<class Y>
    CurveSegment<Y>::~CurveSegment()
    {
    }
}

// Explicit instantiation
template class OSSIA_EXPORT OSSIA::CurveSegment<bool>;
template class OSSIA_EXPORT OSSIA::CurveSegment<int>;
template class OSSIA_EXPORT OSSIA::CurveSegment<float>;

template class OSSIA_EXPORT OSSIA::Curve<double, bool>;
template class OSSIA_EXPORT OSSIA::Curve<double, int>;
template class OSSIA_EXPORT OSSIA::Curve<double, float>;

template class OSSIA_EXPORT OSSIA::Curve<bool, bool>;
template class OSSIA_EXPORT OSSIA::Curve<bool, int>;
template class OSSIA_EXPORT OSSIA::Curve<bool, float>;

template class OSSIA_EXPORT OSSIA::Curve<int, bool>;
template class OSSIA_EXPORT OSSIA::Curve<int, int>;
template class OSSIA_EXPORT OSSIA::Curve<int, float>;

template class OSSIA_EXPORT OSSIA::Curve<float, bool>;
template class OSSIA_EXPORT OSSIA::Curve<float, int>;
template class OSSIA_EXPORT OSSIA::Curve<float, float>;


