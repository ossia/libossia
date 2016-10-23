#pragma once
#include <eggs/variant.hpp>
#include <ossia/editor/curve/curve_abstract.hpp>
#include <vector>
#include <memory>
#include <ossia_export.h>

namespace ossia
{

/**
 * @class behavior Contains either no curve, a single curve, or multiple curves
 *
 * \see \ref curve_abstract
 * \see \ref curve
 */
struct OSSIA_EXPORT behavior final :
    public eggs::variant<std::shared_ptr<curve_abstract>, std::vector<behavior>>
{
  using eggs::variant<std::shared_ptr<curve_abstract>, std::vector<behavior>>::variant;

  /**
   * @brief reset Recursively calls reset on the curves of this behavior.
   */
  void reset();
};

}
