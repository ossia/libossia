/*!
 * \file curve.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */
#include <ossia/OSSIA.hpp>
#include <iostream>

using namespace ossia;
using namespace std;

int main()
{
  auto c = curve<double, float>::create();
  curve_segment_linear<float> firstCurveSegment;
  curve_segment_linear<float> secondCurveSegment;

  cout << "*** test 1 ***" << endl;
  c->setInitialPointAbscissa(0.);
  c->setInitialPointOrdinate(0.);
  c->addPoint(firstCurveSegment, 1., 1.);
  c->addPoint(secondCurveSegment, 2., 0.);

  cout << "value at 0. = " << c->valueAt(0.) << endl;
  cout << "value at 0.5 = " << c->valueAt(0.5) << endl;
  cout << "value at 1. = " << c->valueAt(1.) << endl;
  cout << "value at 1.5 = " << c->valueAt(1.5) << endl;
  cout << "value at 2. = " << c->valueAt(2.) << endl;

  cout << "*** test 2 ***" << endl;
  c->setInitialPointOrdinate(2.);

  cout << "value at 0. = " << c->valueAt(0.) << endl;
  cout << "value at 0.5 = " << c->valueAt(0.5) << endl;
  cout << "value at 1. = " << c->valueAt(1.) << endl;
  cout << "value at 1.5 = " << c->valueAt(1.5) << endl;
  cout << "value at 2. = " << c->valueAt(2.) << endl;

  // Local device
  ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};

  auto localTupleNode = device.createChild("my_tuple");
  auto localTupleAddress = localTupleNode->createAddress(val_type::TUPLE);

  localTupleAddress->setValue(Tuple{Float(-1.), Float(0.), Float(1.)});

  cout << "*** test 3 ***" << endl;
  c->setInitialPointOrdinateDestination(Destination(*localTupleNode));

  cout << "value at 0. = " << c->valueAt(0.) << endl;
  cout << "value at 0.5 = " << c->valueAt(0.5) << endl;
  cout << "value at 1. = " << c->valueAt(1.) << endl;
  cout << "value at 1.5 = " << c->valueAt(1.5) << endl;
  cout << "value at 2. = " << c->valueAt(2.) << endl;

  cout << "*** test 4 ***" << endl;
  c->setInitialPointOrdinateDestination(Destination(*localTupleNode, {1}));

  cout << "value at 0. = " << c->valueAt(0.) << endl;
  cout << "value at 0.5 = " << c->valueAt(0.5) << endl;
  cout << "value at 1. = " << c->valueAt(1.) << endl;
  cout << "value at 1.5 = " << c->valueAt(1.5) << endl;
  cout << "value at 2. = " << c->valueAt(2.) << endl;
}
