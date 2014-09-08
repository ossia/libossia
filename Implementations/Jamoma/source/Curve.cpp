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

#include "TTCurve.h"

namespace OSSIA {

template <typename T>
class Curve<T>::Impl {

public:
    
  // Jamoma variables
  TTObject mCurve;

  T mInitialValue;
  std::map<double, std::pair<T, CurveSegment&>> mPointsMap;

  Impl() : mInitialValue(), mPointsMap() {};
  Impl(const Impl & other) = default;
  ~Impl() = default;

  std::map<double, std::pair<T, CurveSegment&>> getPointsMap() {
    return mPointsMap;
  }

  bool addPoint(double abscissa, T value, CurveSegment & segment) {
    return mPointsMap.emplace(abscissa, std::make_pair(value, segment)).second;
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
Curve<T> & Curve<T>::operator= (const Curve & other)
{
  delete pimpl;
  pimpl = new Impl(other.pimpl);
}

// Iterators

// ...

template <typename T>
bool Curve<T>::addPoint(double abs, const T value, const CurveSegment & segment)
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
std::map<double, std::pair<T, CurveSegment&>> Curve<T>::getPointsMap() const
{
  return pimpl->getPointsMap();
}

}
