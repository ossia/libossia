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
    
    // update the internal curve object
    mCurve.set("functionParameters", editParameters());
    
    return mPointsMap[abscissa].second;
  }
  
  bool removePoint(double abscissa) {
    
    // update the points map
    if (mPointsMap.erase(abscissa) > 0) {
    
      // update the internal curve object
      mCurve.set("functionParameters", editParameters());
      
      return true;
    }
    
    return false;
  }
  
  // edit parameters for the mPointsMap as a value containing <x1 y1 c1 x2 y2 c2>
  TTValue editParameters()
  {
    TTValue   parameters;
    auto      it = mPointsMap.begin();
    TTUInt32  i = 0;
    
    parameters.resize((mPointsMap + 1) * 3);
    
    // edit x1 y1 c1
    parameters[0] = 0.;
    parameters[1] = mInitialValue;
    if (it->second->second->getType() == CurveSegment<T>::POWER_TYPE)
      // TODO : parameters[i+2] = TTFloat64(ExponentialCurveSegment(it->second->second)->getCoefficient());
      parameters[2] = TTFloat64(1.);
    else
      parameters[2] = TTFloat64(1.);
    
    // edit x2 y2 c2 x3 y3 c3 ...
    // note : the coefficient is into the next curve segment
    i++;
    
    for (; it != mPointsMap.end();)
    {
      parameters[i] = TTFloat64(it->first);
      parameters[i+1] = TTFloat64(it->second->first);
      
      // go to next curve segment
      it++;
      
      if (it->second->second->getType() == CurveSegment<T>::POWER_TYPE)
        // TODO : parameters[i+2] = TTFloat64(ExponentialCurveSegment(it->second->second)->getCoefficient());
        parameters[i+2] = TTFloat64(1.);
      else
        parameters[i+2] = TTFloat64(1.);
      
      i++;
    }
    
    return parameters;
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
  return pimpl->mInitialValue;
}

template <typename T>
void Curve<T>::setInitialValue(const T value)
{
  pimpl->mInitialValue = value;
}

template <typename T>
std::map<double, std::pair<T, CurveSegment<T>&>> Curve<T>::getPointsMap() const
{
  return pimpl->getPointsMap();
}

}
