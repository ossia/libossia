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

int main()
{
    auto myCurve = Curve<double>::create();
    auto firstCurveSegment = CurveSegmentLinear<double>::create(myCurve);
    auto secondCurveSegment = CurveSegmentLinear<double>::create(myCurve);

    cout << "*** test 1 ***" << "\n";
    myCurve->setInitialValue(0.);
    myCurve->addPoint(1., 1., firstCurveSegment);
    myCurve->addPoint(2., 0., secondCurveSegment);

    cout << "value at 0. = " << myCurve->valueAt(0.) << "\n";
    cout << "value at 0.5 = " << myCurve->valueAt(0.5) << "\n";
    cout << "value at 1. = " << myCurve->valueAt(1.) << "\n";
    cout << "value at 1.5 = " << myCurve->valueAt(1.5) << "\n";
    cout << "value at 2. = " << myCurve->valueAt(2.) << "\n";

    cout << "*** test 2 ***" << "\n";
    myCurve->setInitialValue(2.);

    cout << "value at 0. = " << myCurve->valueAt(0.) << "\n";
    cout << "value at 0.5 = " << myCurve->valueAt(0.5) << "\n";
    cout << "value at 1. = " << myCurve->valueAt(1.) << "\n";
    cout << "value at 1.5 = " << myCurve->valueAt(1.5) << "\n";
    cout << "value at 2. = " << myCurve->valueAt(2.) << "\n";
}
