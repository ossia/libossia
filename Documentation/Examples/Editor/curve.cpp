// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*!
 * \file curve.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */
#include <ossia/ossia.hpp>
#include <iostream>

using namespace ossia;
using namespace std;

int main()
{
  auto c = std::make_shared<curve<double, float>>();
  curve_segment_linear<float> firstCurveSegment;
  curve_segment_linear<float> secondCurveSegment;

  cout << "*** test 1 ***" << endl;
  c->set_x0(0.);
  c->set_y0(0.);
  c->add_point(firstCurveSegment, 1., 1.);
  c->add_point(secondCurveSegment, 2., 0.);

  cout << "value at 0. = " << c->value_at(0.) << endl;
  cout << "value at 0.5 = " << c->value_at(0.5) << endl;
  cout << "value at 1. = " << c->value_at(1.) << endl;
  cout << "value at 1.5 = " << c->value_at(1.5) << endl;
  cout << "value at 2. = " << c->value_at(2.) << endl;

  cout << "*** test 2 ***" << endl;
  c->set_y0(2.);

  cout << "value at 0. = " << c->value_at(0.) << endl;
  cout << "value at 0.5 = " << c->value_at(0.5) << endl;
  cout << "value at 1. = " << c->value_at(1.) << endl;
  cout << "value at 1.5 = " << c->value_at(1.5) << endl;
  cout << "value at 2. = " << c->value_at(2.) << endl;

  // Local device
  ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

  auto localTupleNode = device.create_child("my_tuple");
  auto localTupleAddress = localTupleNode->create_parameter(val_type::TUPLE);

  localTupleAddress->set_value(std::vector<ossia::value>{-1., 0., 1.});

  cout << "*** test 3 ***" << endl;
  c->set_y0_destination(Destination(*localTupleAddress));

  cout << "value at 0. = " << c->value_at(0.) << endl;
  cout << "value at 0.5 = " << c->value_at(0.5) << endl;
  cout << "value at 1. = " << c->value_at(1.) << endl;
  cout << "value at 1.5 = " << c->value_at(1.5) << endl;
  cout << "value at 2. = " << c->value_at(2.) << endl;

  cout << "*** test 4 ***" << endl;
  c->set_y0_destination(Destination(*localTupleAddress, destination_index{1}));

  cout << "value at 0. = " << c->value_at(0.) << endl;
  cout << "value at 0.5 = " << c->value_at(0.5) << endl;
  cout << "value at 1. = " << c->value_at(1.) << endl;
  cout << "value at 1.5 = " << c->value_at(1.5) << endl;
  cout << "value at 2. = " << c->value_at(2.) << endl;
}
