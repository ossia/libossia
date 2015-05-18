#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"

using namespace OSSIA;
using namespace std;

class MockCurve : public Curve<double>
{
    public:
        MockCurve();
        MockCurve(const MockCurve & curve) : Curve<double>(curve) {}
        shared_ptr<Curve> clone() const {return shared_ptr<Curve>(new MockCurve(*this));}
        // Iterators
        class const_iterator; // bidirectional
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator find(double) const;

        // Manage points (abscissa in double between 0. and 1.)
        bool addPoint(double, double, CurveSegment<double>&) {return true;}
        bool removePoint(double) {return true;}

        // Accessors
        double getInitialValue() const {return 0.5;}
        void setInitialValue(const double) {}
        std::map<double, std::pair<double, CurveSegment<double>>> getPointsMap() const {return {{0.1, {0.5, {}}}};}
          // {abscissa, {value, previous segment}}

        // Computation
        double valueAt(double) const {return 0.5;} // Between 0. and 1.

};

namespace OSSIA
{
    template<> shared_ptr<Curve<double> > Curve<double>::create() {
        return shared_ptr<Curve>(new MockCurve());}
}
