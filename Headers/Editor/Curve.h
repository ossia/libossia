/*!
 * \file Curve.h
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

#pragma once

#include <map>
#include <utility>
#include <memory>
#include "Misc/Container.h"

namespace OSSIA {

template <typename T>
class CurveSegment;

template <typename T>
class Curve {

    public:
      typedef T value_type;

      static std::shared_ptr<Curve> create();
      virtual std::shared_ptr<Curve> clone() const = 0;
      virtual ~Curve() = default;

      // Manage points (abscissa in double between 0. and 1.)
      virtual bool addPoint(double, T, CurveSegment<T>&) = 0;
      virtual bool removePoint(double) = 0;

    # pragma mark -
# pragma mark Accessors
      virtual T getInitialValue() const = 0;
      virtual void setInitialValue(const T) = 0;
      virtual std::map<double, std::pair<T, std::shared_ptr<CurveSegment<T> > > > getPointsMap() const = 0;
        // {abscissa, {value, previous segment}}

      // Computation
      virtual T valueAt(double) const = 0; // Between 0. and 1.

      Container<CurveSegment<T>>& curveSegments()
      { return m_curveSegments; }
      const Container<CurveSegment<T>>& curveSegments() const
      { return m_curveSegments; }

    private:
      Container<CurveSegment<T>> m_curveSegments;
};

}
