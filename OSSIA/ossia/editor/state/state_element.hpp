#pragma once
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/state/state_element_fwd.hpp>
#include <ossia_export.h>

/**
 * \file state_element.hpp
 */
namespace ossia
{
/**
 * @brief launch Launch a \ref state_element
 */
OSSIA_EXPORT void launch(state_element& e);

/**
 * @brief print Print a \ref state_element
 */
OSSIA_EXPORT std::ostream& print(std::ostream& os, const state_element& dt);

inline
ossia::state_element& get_state_element(std::vector<ossia::state_element>::iterator iterator)
{
  return *iterator;
}
}
