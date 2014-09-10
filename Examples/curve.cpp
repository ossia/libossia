/*!
 * \file curve.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"
#include "Editor/CurveSegment/CurveSegmentNone.h"
#include "Editor/CurveSegment/CurveSegmentLinear.h"

using namespace OSSIA;

int main() {

    Curve myCurve;
    CurveSegmentLinear firstCurveSegment(myCurve);
    
    myCurve.setInitialeValue(0.);
    myCurve.addPoint(1., 1., firstCurveSegment);
    
    std::cout << "value at 0. = " << myCurve.valueAt(0.) << "\n";
}
