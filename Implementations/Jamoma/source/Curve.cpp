/*!
 * \file Curve.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"

#include "TTCurve.h"

namespace OSSIA {

template <typename T>
class Curve<T>::Impl {

public:
  
  TTObject mCurve;

  T mInitialValue;
  std::map<double, std::pair<T, CurveSegment<T>&>> mPointsMap;

  Impl() : mCurve("Curve"), mInitialValue(), mPointsMap() {};
  
  Impl(const Impl & other) = default;
  ~Impl() = default;

  std::map<double, std::pair<T, CurveSegment<T>&>> getPointsMap() {
    return mPointsMap;
  }

  bool addPoint(double abscissa, T value, CurveSegment<T> & segment) {
    
    // update the points map
    mPointsMap.emplace(abscissa, std::make_pair(value, segment));
    
    // edit parameters as x1 y1 b1 x2 y2 b2
    TTValue   parameters;
    TTUInt32  i = 0;
    
    parameters.resize(mPointsMap * 3);
    
    for (auto it = mPointsMap.begin(); it != mPointsMap.end(); it++)
    {
      parameters[i] = TTFloat64(it->first);
      parameters[i+1] = TTFloat64(it->second->first);

      if (it->second->second->getType() == CurveSegment<T>::POWER_TYPE)
        // TODO : parameters[i+2] = TTFloat64(ExponentialCurveSegment(it->second->second)->getCoefficient());
        parameters[i+2] = TTFloat64(1.);
      else
        parameters[i+2] = TTFloat64(1.);
      
      i++;
    }
    
    // update the internal curve object
    mCurve.set("functionParameters", parameters);
    
    return mPointsMap[abscissa].second;
  }
  
  bool removePoint(double abscissa) {
    return mPointsMap.erase(abscissa) > 0;
  }
  
};

template <typename T>
Curve<T>::Curve() :
    pimpl(new Impl)
{}

template <typename T>
Curve<T>::Curve(const Curve & other) :
    pimpl(new Impl(other.pimpl))
{}

template <typename T>
Curve<T>::~Curve()
{
  delete pimpl;
}

template <typename T>
Curve<T>& Curve<T>::operator= (const Curve & other)
{
  delete pimpl;
  pimpl = new Impl(other.pimpl);
}

// Iterators

// todo ...

template <typename T>
bool Curve<T>::addPoint(double abs, const T value, const CurveSegment<T> & segment)
{
  return pimpl->addPoint(abs, value, segment);
}

template <typename T>
bool Curve<T>::removePoint(double abs)
{
  return pimpl->removePoint(abs);
}

template <typename T>
T Curve<T>::getInitialValue() const
{
  return pimpl->initialValue;
}

template <typename T>
void Curve<T>::setInitialValue(const T value)
{
  pimpl->initialValue = value;
}

template <typename T>
std::map<double, std::pair<T, CurveSegment<T>&>> Curve<T>::getPointsMap() const
{
  return pimpl->getPointsMap();
}

}
