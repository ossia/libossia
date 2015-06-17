/*!
 * \file Curve.cpp
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Curve.h"

#include "TTScore.h"

#include "Editor/CurveSegment.h"

#include "../Implementations/Jamoma/Sources/Editor/CurveSegment.cpp" // because we use the parent curve into the segment (see : valueAt)

namespace OSSIA {
  
template <typename T>
class Curve<T>::Impl {
  
public:
  
  TTObject mCurve;
  
  T mInitialValue;
  map<double, pair<T, CurveSegment<T>&>> mPointsMap;
  
  Impl() : /*mCurve("Curve"),*/ mInitialValue(), mPointsMap()
  {
    // todo : move this else where ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
    
    mCurve = TTObject("Curve");
  };
  
  Impl(const Impl & other) = default;
  ~Impl() = default;
  
  // edit parameters for the mPointsMap as a value containing <x1 y1 c1 x2 y2 c2 x3 y3 c3 ...>
  TTValue editParameters()
  {
    TTValue   parameters;
    auto      it = mPointsMap.begin();
    
    parameters.resize((mPointsMap.size() + 1) * 3);
    
    for (TTUInt32 i = 0; i <= mPointsMap.size(); i++)
    {
      // edit xi yi
      if (i == 0) {
          
        parameters[0] = 0.;
        parameters[1] = mInitialValue;
      }
      else {
        
        parameters[i*3] = TTFloat64(it->first);
        parameters[i*3+1] = TTFloat64(it->second.first);
          
        // go to next curve segment
        it++;
      }
        
      if (it == mPointsMap.end()) {
        parameters[i*3+2] = TTFloat64(1.);
        break;
      }
      
      // edit ci
      if (it->second.second.getType() == CurveSegment<T>::CurveSegmentType::POWER)
        // TODO : parameters[i+2] = TTFloat64(ExponentialCurveSegment(it->second->second)->getCoefficient());
        parameters[i*3+2] = TTFloat64(1.);
      else
        parameters[i*3+2] = TTFloat64(1.);
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
pimpl(new Impl(*(other.pimpl)))
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
  pimpl = new Impl(*(other.pimpl));
  return *this;
}

// Iterators

// todo ...

template <typename T>
bool Curve<T>::addPoint(double abscissa, T value, CurveSegment<T> & segment)
{
  pair <T,CurveSegment<T>&> pair(value, segment);
  
  // update the points map
  pimpl->mPointsMap.emplace(abscissa, pair);
  
  // update the internal curve object
  return pimpl->mCurve.set("functionParameters", pimpl->editParameters()) == kTTErrNone;
}

template <typename T>
bool Curve<T>::removePoint(double abscissa)
{
  // update the points map
  if (pimpl->mPointsMap.erase(abscissa) > 0) {
    
    // update the internal curve object
    pimpl->mCurve.set("functionParameters", pimpl->editParameters());
    
    return true;
  }
  
  return false;
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
  
  // update the internal curve object
  pimpl->mCurve.set("functionParameters", pimpl->editParameters());
}

template <typename T>
map<double, pair<T, CurveSegment<T>&>> Curve<T>::getPointsMap() const
{
  return pimpl->mPointsMap;
}

template <typename T>
T Curve<T>::valueAt(double abscissa) const
{
  if (pimpl->mPointsMap.size() > 0) {
    TTValue out;
    pimpl->mCurve.send("ValueAt", TTFloat64(abscissa), out);
    return T(out[0]);
  }
    
  return pimpl->mInitialValue;
}

// explicit instantiation for double
template class Curve<double>;

}
