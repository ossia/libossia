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
#include "Editor/CurveSegment/CurveSegmentLinear.h"

using namespace OSSIA;

#include <iostream>

using namespace std;

int main() {

    Curve<double> myCurve;
    CurveSegmentLinear<double> firstCurveSegment(&myCurve);
    
    myCurve.setInitialValue(0.);
    myCurve.addPoint(1., 1., firstCurveSegment);
    
    cout << "value at 0. = " << myCurve.valueAt(0.) << "\n";
    cout << "value at 0.5 = " << myCurve.valueAt(0.5) << "\n";
    cout << "value at 1. = " << myCurve.valueAt(1.) << "\n";
}
