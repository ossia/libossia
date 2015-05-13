#include "Editor/CurveSegment/CurveSegmentLinear.h"
#include "Editor/CurveSegment.h"
#include "Editor/Curve.h"

using namespace OSSIA;

class MockCurveSegmentLinear : public CurveSegmentLinear<double>
{
    MockCurveSegmentLinear(Curve<double>* curve) : CurveSegmentLinear<double>{curve} {}
    MockCurveSegmentLinear(const MockCurveSegmentLinear& lin) : CurveSegmentLinear{lin} {}
    MockCurveSegmentLinear & operator=(const MockCurveSegmentLinear&) {return *this;}

    double valueAt(double) const {return 0.5;}
};
