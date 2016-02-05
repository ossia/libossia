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
#include "Editor/Value.h"

#include "Network/Address.h"
#include "Network/Device.h"
#include "Network/Node.h"
#include "Network/Protocol/Local.h"

using namespace OSSIA;

#include <iostream>

using namespace std;

int main()
{
  auto myCurve = Curve<double, float>::create();
  auto firstCurveSegment = CurveSegmentLinear<float>::create(myCurve);
  auto secondCurveSegment = CurveSegmentLinear<float>::create(myCurve);
  
  cout << "*** test 1 ***" << endl;
  myCurve->setInitialValue(0.);
  myCurve->addPoint(firstCurveSegment, 1., 1.);
  myCurve->addPoint(secondCurveSegment, 2., 0.);
  
  cout << "value at 0. = " << myCurve->valueAt(0.) << endl;
  cout << "value at 0.5 = " << myCurve->valueAt(0.5) << endl;
  cout << "value at 1. = " << myCurve->valueAt(1.) << endl;
  cout << "value at 1.5 = " << myCurve->valueAt(1.5) << endl;
  cout << "value at 2. = " << myCurve->valueAt(2.) << endl;
  
  cout << "*** test 2 ***" << endl;
  myCurve->setInitialValue(2.);
  
  cout << "value at 0. = " << myCurve->valueAt(0.) << endl;
  cout << "value at 0.5 = " << myCurve->valueAt(0.5) << endl;
  cout << "value at 1. = " << myCurve->valueAt(1.) << endl;
  cout << "value at 1.5 = " << myCurve->valueAt(1.5) << endl;
  cout << "value at 2. = " << myCurve->valueAt(2.) << endl;
  
  // Local device
  auto local_protocol = Local::create();
  auto device = Device::create(local_protocol, "test");
  
  auto localTupleNode = *(device->emplace(device->children().cend(), "my_tuple"));
  auto localTupleAddress = localTupleNode->createAddress(Value::Type::TUPLE);
  
  Tuple t = {new Float(-1.), new Float(0.), new Float(1.)};
  localTupleAddress->setValue(&t);

  cout << "*** test 3 ***" << endl;
  Destination d1(localTupleNode);
  myCurve->setInitialDestination(&d1);
  
  cout << "value at 0. = " << myCurve->valueAt(0.) << endl;
  cout << "value at 0.5 = " << myCurve->valueAt(0.5) << endl;
  cout << "value at 1. = " << myCurve->valueAt(1.) << endl;
  cout << "value at 1.5 = " << myCurve->valueAt(1.5) << endl;
  cout << "value at 2. = " << myCurve->valueAt(2.) << endl;
  
  cout << "*** test 4 ***" << endl;
  Destination d2(localTupleNode, {1});
  myCurve->setInitialDestination(&d2);
  
  cout << "value at 0. = " << myCurve->valueAt(0.) << endl;
  cout << "value at 0.5 = " << myCurve->valueAt(0.5) << endl;
  cout << "value at 1. = " << myCurve->valueAt(1.) << endl;
  cout << "value at 1.5 = " << myCurve->valueAt(1.5) << endl;
  cout << "value at 2. = " << myCurve->valueAt(2.) << endl;
}
