#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"

using namespace OSSIA;

class MockCurveSegment : public CurveSegment<double>
{
    public:
        MockCurveSegment() {mParent = nullptr;}
        MockCurveSegment(Curve<double>* parent) {mParent = parent;}
        virtual ~MockCurveSegment() {}

        // Navigation
        Curve<double>* getParent() const {return mParent;}

        // Computation
        virtual double valueAt(double) const // Between 0. and 1.
        {
          return 0.0;
        }

        virtual CurveSegmentType getType() const
        {return CurveSegment<double>::CurveSegmentType::NONE;}
};
