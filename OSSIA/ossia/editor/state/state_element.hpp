#pragma once
#include <Editor/State/State.hpp>
#include <Editor/State/CustomState.hpp>
#include <Editor/State/Message.hpp>

#include <eggs/variant.hpp>

#include <ossia_export.h>

namespace OSSIA
{
using StateElement = eggs::variant<Message, State, CustomState>;

OSSIA_EXPORT void launch(const StateElement& e);
}
