#pragma once
#include <eggs/variant.hpp>
namespace ossia
{
struct Message;
class State;
class CustomState;
using StateElement = eggs::variant<Message, State, CustomState>;
}
