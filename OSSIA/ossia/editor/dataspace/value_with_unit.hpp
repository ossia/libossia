#pragma once
#include <ossia/editor/dataspace/dataspace_fwd.hpp>
#include <brigand/algorithms/fold.hpp>
namespace ossia
{

// Basically eggs::variant<ossia::value, ossia::distance, ossia::position, ossia::speed...>
using value_with_unit_variant =
brigand::as_eggs_variant<
  brigand::append<
    brigand::list<ossia::value>,
    brigand::as_list<
      brigand::transform<
        unit_variant,
        brigand::bind<ossia::add_value, brigand::_1>
      >
    >
  >
>;

struct OSSIA_EXPORT value_with_unit final : public value_with_unit_variant
{
  OSSIA_DECL_RELAXED_CONSTEXPR value_with_unit() noexcept = default;

#if defined(_MSC_VER)
  template<typename T>
  value_with_unit(const T& arg): value_with_unit_variant(arg) { }
  value_with_unit(const value_with_unit& d) : value_with_unit_variant{ (const value_with_unit_variant&)d } { }
  value_with_unit(value_with_unit&& d) : value_with_unit_variant{ std::move((value_with_unit_variant&)d) } { }
  value_with_unit& operator=(const value_with_unit& d) { ((value_with_unit_variant&)(*this)) = (const value_with_unit_variant&)d; return *this; }
  value_with_unit& operator=(value_with_unit&& d) { ((value_with_unit_variant&)(*this)) = std::move((value_with_unit_variant&)d); return *this; }
  ~value_with_unit() { }
#else
  using value_with_unit_variant::value_with_unit_variant;
  value_with_unit(const value_with_unit&) = default;
  value_with_unit(value_with_unit&&) noexcept = default;
  value_with_unit& operator=(const value_with_unit&) = default;
  value_with_unit& operator=(value_with_unit&&) noexcept = default;
#endif
};

}
