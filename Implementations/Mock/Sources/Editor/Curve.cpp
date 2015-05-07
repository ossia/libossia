#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"

using namespace OSSIA;

class MockCurve : public Curve<double>
{
    public:
        MockCurve();

        // Iterators
        class const_iterator; // bidirectional
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator find(double) const;

        // Manage points (abscissa in double between 0. and 1.)
        bool addPoint(double, double, CurveSegment<double>&) {return true;}
        bool removePoint(double) {return true;}

        // Accessors
        double getInitialValue() const {return 0.0;}
        void setInitialValue(const double) {}
        std::map<double, std::pair<double, CurveSegment<double>>> getPointsMap() const {return {{0.0, {0.0, {}}}};}
          // {abscissa, {value, previous segment}}

        // Computation
        double valueAt(double) const {return 0.0;} // Between 0. and 1.

};
