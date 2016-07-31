#include "Curve_impl.hpp"
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/node.hpp>

namespace detail
{
template <typename X, typename Y>
curve_impl<X,Y>::curve_impl() = default;

template <typename X, typename Y>
curve_impl<X,Y>::~curve_impl() = default;

template <typename X, typename Y>
bool curve_impl<X,Y>::
addPoint(ossia::curve_segment<Y> segment, X abscissa, Y value)
{
  mPointsMap.emplace(abscissa, std::make_pair(value, std::move(segment)));

  return true;
}

template <typename X, typename Y>
bool curve_impl<X,Y>::
removePoint(X abscissa)
{
  return mPointsMap.erase(abscissa) > 0;
}

template <typename X, typename Y>
Y curve_impl<X,Y>::
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
      lastValue = it->second.second(
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

template<typename X, typename Y>
ossia::curve_type curve_impl<X, Y>::getType() const
{
    return std::make_pair(OssiaType<X>, OssiaType<Y>);
}

template <typename X, typename Y>
X curve_impl<X,Y>::
getInitialPointAbscissa() const
{
  auto& node = mInitialPointAbscissaDestination.value;
  if(!node)
      return mInitialPointAbscissa;

  auto address = node->getAddress();

  if (!address)
    throw std::runtime_error("getting an address value using from an abscissa destination without address");

  address->pullValue();
  auto val = address->cloneValue();
  auto res = convertToTemplateTypeValue(val, mInitialPointAbscissaDestination.index.begin());
  return res;
}

template <typename X, typename Y>
Y curve_impl<X,Y>::
getInitialPointOrdinate() const
{
  auto& node = mInitialPointOrdinateDestination.value;
  if(!node)
    return mInitialPointOrdinate;

  if(mInitialPointOrdinateCacheUsed)
    return mInitialPointOrdinateCache;

  auto address = node->getAddress();

  if (!address)
    throw std::runtime_error("getting an address value using from an ordinate destination without address");

  address->pullValue();
  auto val = address->cloneValue();
  mInitialPointOrdinateCacheUsed = true;
  mInitialPointOrdinateCache = convertToTemplateTypeValue(val, mInitialPointOrdinateDestination.index.begin());
  return mInitialPointOrdinateCache;
}

template <typename X, typename Y>
void curve_impl<X,Y>::
setInitialPointAbscissa(X value)
{
  mInitialPointAbscissa = value;
}

template <typename X, typename Y>
void curve_impl<X,Y>::
setInitialPointOrdinate(Y value)
{
  mInitialPointOrdinate = value;
}

template <typename X, typename Y>
const ossia::Destination& curve_impl<X,Y>::
getInitialPointAbscissaDestination() const
{
  return mInitialPointAbscissaDestination;
}

template <typename X, typename Y>
const ossia::Destination& curve_impl<X,Y>::
getInitialPointOrdinateDestination() const
{
  return mInitialPointOrdinateDestination;
}

template <typename X, typename Y>
void curve_impl<X,Y>::
setInitialPointAbscissaDestination(const ossia::Destination& destination)
{
  mInitialPointAbscissaDestination = destination;
}

template <typename X, typename Y>
void curve_impl<X,Y>::
setInitialPointOrdinateDestination(const ossia::Destination& destination)
{
  mInitialPointOrdinateDestination = destination;
}

template <typename X, typename Y>
std::map<X, std::pair<Y, ossia::curve_segment<Y>>> curve_impl<X,Y>::
getPointsMap() const
{
    return {mPointsMap.cbegin(), mPointsMap.cend()};
}

template <typename X, typename Y>
Y curve_impl<X,Y>::
convertToTemplateTypeValue(
    const ossia::value& value,
    ossia::destination_index::const_iterator idx)
{
  using namespace ossia;
  using namespace std;
  struct visitor {
    destination_index::const_iterator index;
    Y operator()(Int i) const   { return i.value; }
    Y operator()(Float f) const { return f.value; }
    Y operator()(Bool b) const  { return b.value; }
    Y operator()(Char c) const  { return c.value; }
    Y operator()(Vec2f vec) const { return vec.value[*index]; }
    Y operator()(Vec3f vec) const { return vec.value[*index]; }
    Y operator()(Vec4f vec) const { return vec.value[*index]; }
    Y operator()(const Tuple& t) const {
      auto& val = t.value[*index];
      return convertToTemplateTypeValue(val, index + 1);
    }

    Y operator()(Impulse) const { throw std::runtime_error("Cannot convert to a numeric type"); }
    Y operator()(const String& str) const { throw std::runtime_error("Cannot convert to a numeric type"); }
    Y operator()(const Destination& d) const { throw std::runtime_error("Cannot convert to a numeric type");; }
    Y operator()(const Behavior&) const { throw std::runtime_error("Cannot convert to a numeric type"); }
  } vis{idx};

  if(value.valid())
    return eggs::variants::apply(vis, value.v);
  throw std::runtime_error("Invalid variant");
}

}
