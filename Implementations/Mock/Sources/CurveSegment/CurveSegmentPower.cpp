#include "Editor/CurveSegment/CurveSegmentPower.h"
#include "Editor/CurveSegment.h"
#include "Editor/Curve.h"

using namespace OSSIA;

class MockCurveSegmentPower : public CurveSegmentPower<double>
{
    MockCurveSegmentPower(Curve<double>* curve) : CurveSegmentPower<double>{curve} {}
    MockCurveSegmentPower(const MockCurveSegmentPower& lin) : CurveSegmentPower{lin} {}
    MockCurveSegmentPower & operator=(const MockCurveSegmentPower&) {return *this;}

    double valueAt(double) const {return 0.5;}
};

