#pragma once
#include <eggs/variant.hpp>
namespace ossia
{
struct message;
class state;
class custom_state;
using state_element = eggs::variant<message, state, custom_state>;
}
