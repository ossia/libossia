#include "Editor/Curve.h"
#include "Editor/CurveSegment.h"
#include "../Misc/Container.cpp"
#include "../Editor/CurveSegment.cpp"

using namespace OSSIA;
using namespace std;

class MockCurve : public Curve<double>, public MockContainer<CurveSegment<double>>
{
    public:
        MockCurve() {}
        MockCurve(const MockCurve * curve) : Curve<double>(*curve) {}
        virtual ~MockCurve() {}

        virtual shared_ptr<Curve<double>> clone() const override {
            return shared_ptr<Curve<double>>(new MockCurve(this));
        }

        // Manage points (abscissa in double between 0. and 1.)
        virtual bool addPoint(double, double, CurveSegment<double>&) override {return true;}
        virtual bool removePoint(double) override {return true;}

        // Accessors
        virtual double getInitialValue() const override {return 0.5;}
        virtual void setInitialValue(const double) override {}
        virtual std::map<double, std::pair<double, std::shared_ptr<CurveSegment<double>>>> getPointsMap() const {return {{0.1, {0.5, {} }}};}
          // {abscissa, {value, previous segment}}

        // Computation
        virtual double valueAt(double) const override {return 0.5;} // Between 0. and 1.

};

template<> shared_ptr<Curve<double> > Curve<double>::create() {
    return shared_ptr<Curve<double>>(new MockCurve());}
