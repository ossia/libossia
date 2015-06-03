/*!
 * \file CurveSegmentLinear.h
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

#include "Editor/CurveSegment.h"

namespace OSSIA {

  template <typename T>
  class CurveSegmentLinear : public CurveSegment<T> {

  public:

    // Constructors, destructor, assignment
    CurveSegmentLinear(Curve<T>*);
    CurveSegmentLinear(const CurveSegmentLinear&);
    virtual ~CurveSegmentLinear();
    CurveSegmentLinear & operator= (const CurveSegmentLinear&);

    // Computation
    virtual T valueAt(double) const override;

    // Curve segment type
    virtual typename CurveSegment<T>::CurveSegmentType getType() const override final
    {return CurveSegment<T>::CurveSegmentType::LINEAR;}

  };

}
