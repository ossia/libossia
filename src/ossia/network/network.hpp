#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/local/local.hpp>

#ifdef OSSIA_PROTOCOL_MINUIT
#include <ossia/network/minuit/minuit.hpp>
#endif

#ifdef OSSIA_PROTOCOL_OSC
#include <ossia/network/osc/osc.hpp>
#endif