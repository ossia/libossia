#pragma once
#include <eggs/variant.hpp>
namespace OSSIA
{
struct Message;
class State;
class CustomState;
using StateElement = eggs::variant<Message, State, CustomState>;
}
