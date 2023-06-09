#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/editor/scenario/time_signature.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <smallfun.hpp>
namespace ossia
{
// Used to notify an external transport system of the current state of the
// transport in the score top-level interval.
struct tick_transport_info;
using transport_info_fun = smallfun::function<void(const tick_transport_info&)>;

struct tick_transport_info
{
  time_value date{};
  double current_tempo{};
  time_signature signature{};

  quarter_note musical_start_last_signature{};

  quarter_note musical_start_last_bar{};
  quarter_note musical_start_position{};

  quarter_note musical_end_last_bar{};
  quarter_note musical_end_position{};

  transport_info_fun update_transport_info;
};

}
