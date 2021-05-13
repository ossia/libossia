#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/message_origin_identifier.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/osc/detail/osc.hpp>

#include <oscpack/osc/OscPrintReceivedElements.h>
#include <oscpack/osc/OscReceivedElements.h>
#include <ossia/detail/fmt.hpp>
#include <fmt/ostream.h>
namespace ossia
{
namespace net
{

struct osc_message_applier
{
  message_origin_identifier id;
  const oscpack::ReceivedMessage& m;
  void on_listened_value(
      ossia::net::parameter_base& the_addr,
      ossia::net::device_base& dev,
      network_logger& logger
      )
  {
    if (auto v = net::get_filtered_value(the_addr, m); v.valid())
    {
      dev.apply_incoming_message(id, the_addr, std::move(v));
    }

    if (logger.inbound_listened_logger)
      logger.inbound_listened_logger->info("[input] {}", m);
  }

  void on_value(
      ossia::net::parameter_base& the_addr,
      ossia::net::device_base& dev
      )
  {
    if (auto v = net::get_filtered_value(the_addr, m); v.valid())
    {
      dev.apply_incoming_message(id, the_addr, std::move(v));
    }
  }

  void on_value_quiet(
      ossia::net::parameter_base& the_addr,
      ossia::net::device_base& dev
      )
  {
    if (auto v = net::get_filtered_value(the_addr, m); v.valid())
    {
      dev.apply_incoming_message_quiet(id, the_addr, std::move(v));
    }
  }

  void on_unhandled(ossia::net::device_base& dev)
  {
    dev.on_unhandled_message(
        m.AddressPattern(),
          net::osc_utilities::create_any(
            m.ArgumentsBegin(), m.ArgumentsEnd(),
            m.ArgumentCount()));
  }

  void log(network_logger& logger)
  {
    if (logger.inbound_logger)
      logger.inbound_logger->info("[input] {}", m);
  }
};

template<bool SilentUpdate, typename F>
void on_input_message(
    std::string_view addr_txt,
    F&& f,
    const ossia::net::listened_parameters& listening,
    ossia::net::device_base& dev,
    network_logger& logger)
{
  auto addr = listening.find(addr_txt);

  if (addr && *addr)
  {
    f.on_listened_value(**addr, dev, logger);
  }
  else
  {
    // We still want to save the value even if it is not listened to.
    if (auto n = find_node(dev.get_root_node(), addr_txt))
    {
      if (auto base_addr = n->get_parameter())
      {
        if constexpr (!SilentUpdate)
          f.on_value(*base_addr, dev);
        else
          f.on_value_quiet(*base_addr, dev);
      }
    }
    else
    {
      // Try to handle pattern matching
      auto nodes = find_nodes(dev.get_root_node(), addr_txt);
      for (auto n : nodes)
      {
        if (auto addr = n->get_parameter())
        {
          if (!SilentUpdate || listening.find(n->osc_address()))
            f.on_value(*addr, dev);
          else
            f.on_value_quiet(*addr, dev);
        }
      }

      if (nodes.empty())
        f.on_unhandled(dev);
    }
  }

  f.log(logger);
}

}
}
