#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"

using namespace OSSIA;

class MockCurveSegment : public CurveSegment<double>
{
    public:
        MockCurveSegment() {}
        MockCurveSegment(Curve<double>* parent) {}
        virtual ~MockCurveSegment() {}

        // Navigation
        virtual std::shared_ptr<Curve<double>> getParent() const
            {return Curve<double>::create();}

        // Computation
        virtual double valueAt(double) const // Between 0. and 1.
            {return 0.5;}

        virtual CurveSegmentType getType() const
            {return CurveSegment<double>::CurveSegmentType::NONE;}
};
