/*!
 * \file CurveSegment.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

namespace OSSIA {

  template <typename T>
  class Curve;

  template <typename T>
  class CurveSegment {

  public:

    // Constructor, Destructor
    CurveSegment();
    CurveSegment(Curve<T> * parent);
    virtual ~CurveSegment();

    // Navigation
    Curve<T>* getParent() const;

    // Computation
    virtual T valueAt(double) const; // Between 0. and 1.

    // Curve segment types
    enum class CurveSegmentType {
      NONE,
      LINEAR,
      POWER
    };

    virtual CurveSegmentType getType() const;

  protected:

    Curve<T>* mParent{};

  };

}

