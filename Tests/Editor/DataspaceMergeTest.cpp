#include <ossia/editor/dataspace/detail/dataspace_merge.hpp>
#include <QtTest>
class DataspaceMergeTest : public QObject
{
  Q_OBJECT

private Q_SLOTS:
void static_test()
{
    static_assert(ossia::detail::is_iterable_v<decltype(ossia::Vec3f::value)>, "");
    static_assert(!ossia::detail::is_iterable_v<decltype(ossia::Float::value)>, "");
    static_assert(!ossia::detail::is_iterable_v<const decltype(ossia::Float::value)>, "");

    static_assert(!ossia::detail::is_iterable_v<decltype(ossia::centimeter::value)>, "");
    static_assert(ossia::detail::is_iterable_v<decltype(ossia::rgb::value)>, "RGB is not iterable");

    static_assert(!ossia::is_unit_v<int>, "");
    static_assert(ossia::is_unit_v<ossia::centimeter_u>, "");
    static_assert(ossia::is_unit_v<ossia::rgb_u>, "");
    static_assert(!ossia::is_unit_v<ossia::color_u>, "");
}
};

QTEST_APPLESS_MAIN(DataspaceMergeTest)

#include "DataspaceMergeTest.moc"
