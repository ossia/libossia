/*!
 * \file CurveSegment.h
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
#include <memory>

namespace OSSIA {

  template <typename T>
  class Curve;

  template <typename T>
  class CurveSegment {

  public:

    // Constructor, Destructor
    virtual ~CurveSegment() = default;

    # pragma mark -
# pragma mark Navigation
    virtual std::shared_ptr<Curve<T>> getParent() const = 0;

    // Computation
    virtual T valueAt(double) const = 0; // Between 0. and 1.

    // Curve segment types
    enum class CurveSegmentType {
      NONE,
      LINEAR,
      POWER
    };

    virtual CurveSegmentType getType() const = 0;

  };

}

