#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>

using namespace ossia;

namespace ossia
{

struct address_data
{
  dataspace_unit_t unit;
  ossia::value value;

  ossia::value_with_unit val;
};

}


class DataspaceTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  /*! test impulse */
  void test_dataspace()
  {
    // Dataspace : enforces a family of units
    // Unit : enforces a certain type of storage

    constexpr ossia::centimeter c{{2.3}};

    constexpr ossia::millimeter m = c;

    static_assert(m.val.value == 23., "");

    static_assert(qFuzzyCompare(ossia::centimeter{ossia::inch{1}}.val.value, 2.54f), "");
    static_assert(qFuzzyCompare(ossia::meter{ossia::mile{1}}.val.value, 1609.34f), "");


    static_assert(qFuzzyCompare(ossia::kilometer_per_hour{ossia::meter_per_second{1}}.val.value, 3.6f), "");
    static_assert(qFuzzyCompare(ossia::miles_per_hour{ossia::meter_per_second{1}}.val.value, 2.236936f), "");
    static_assert(qFuzzyCompare(ossia::foot_per_second{ossia::meter_per_second{1}}.val.value, 3.280840f), "");
    static_assert(qFuzzyCompare(ossia::knot{ossia::meter_per_second{1}}.val.value, 1.943844f), "");

    static_assert(qFuzzyCompare(ossia::radian{ossia::degree{180}}.val.value, 3.14159f), "");
    // Ex. 1 : making an automation of the correct type ? e.g. circular for circular units...

    // have some kind of unit transformer ?
    // an automation on circular units should yield a circle.
    // functionnally : autom value -> unit transformer -> address
    // we are interested in :
    // - units as metadata to show up the correct widgets, etc
    // - unit transformation functions to do meaningful automations
    // - units as a c++ type to write stuff using the ossia api.

    // the unit stored in the address is the unit in which what we currently have must be converted into.
    // what should happen if there is a type error ?
    // if we push a simple "ossia::value" and its underlying type matches the unit's value_type,
    // we assume it is of the correct unit
    // if we push a value of the same dataspace, we convert it

    // Maybe the "unit" types shouldn't hold a value, but just have a value_type member ?
  }
};

QTEST_APPLESS_MAIN(DataspaceTest)

#include "DataspaceTest.moc"
