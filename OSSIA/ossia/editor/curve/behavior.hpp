#pragma once
#include <eggs/variant.hpp>
#include <ossia/editor/curve/curve_abstract.hpp>
#include <vector>
#include <memory>
#include <ossia_export.h>

namespace ossia
{

struct behavior;
using behavior_variant_type = eggs::variant<std::shared_ptr<curve_abstract>, std::vector<behavior>>;
/**
 * @class behavior Contains either no curve, a single curve, or multiple curves
 *
 * \see \ref curve_abstract
 * \see \ref curve
 */
struct OSSIA_EXPORT behavior final :
    public behavior_variant_type
{
#if defined(_MSC_VER)
  template<typename... Args>
  behavior(Args&&... args) : behavior_variant_type(std::forward<Args>(args)...) { }
#else
  using behavior_variant_type::behavior_variant_type;
#endif

  /**
   * @brief reset Recursively calls reset on the curves of this behavior.
   */
  void reset();
};

}
