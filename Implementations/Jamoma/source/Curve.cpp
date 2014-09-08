/*!
 * \file Curve.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "../../../Curve.h"

namespace OSSIA {

template <typename T>
class Curve<T>::Impl {

public:

  T initialValue;
  std::map<double, std::pair<T, CurveSegment&>> pointsMap;

  Impl() : initialValue(), pointsMap() {};
  Impl(const Impl & other) = default;
  ~Impl() = default;

  std::map<double, std::pair<T, CurveSegment&>> getPointsMap() {
    return pointsMap;
  }

  bool addPoint(double abs, T value, CurveSegment & segment) {
    return pointsMap.emplace(abs, std::make_pair(value, segment)).second;
  }

  bool removePoint(double abs) {
    return pointsMap.erase(abs) > 0;
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
