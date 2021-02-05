#pragma once
#include <cstdint>

namespace ossia::net
{
class protocol_base;

struct message_origin_identifier
{
  ossia::net::protocol_base& protocol;
  uintptr_t identifier{};
};

}
