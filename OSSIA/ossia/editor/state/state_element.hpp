#pragma once
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/custom_state.hpp>
#include <ossia/editor/state/state.hpp>

#include <eggs/variant.hpp>

#include <ossia_export.h>

namespace OSSIA
{
using StateElement = eggs::variant<Message, State, CustomState>;

OSSIA_EXPORT void launch(const StateElement& e);
}
