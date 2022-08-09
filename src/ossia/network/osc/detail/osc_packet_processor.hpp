#pragma once
#include <ossia/detail/logger.hpp>

#include <oscpack/osc/OscReceivedElements.h>

namespace ossia::net
{
template <typename F>
struct osc_packet_processor
{
  F on_message;

  void operator()(const char* data, std::size_t sz) const noexcept
  try
  {
    oscpack::ReceivedPacket p{data, sz};
    if(!p.IsBundle())
      on_message(oscpack::ReceivedMessage(p));
    else
      on_bundle(oscpack::ReceivedBundle(p));
  }
  catch(std::exception& e)
  {
    ossia::logger().error("[osc_packet_processor] {}", e.what());
  }
  catch(...)
  {
    ossia::logger().error("[osc_packet_processor]");
  }

private:
  void on_bundle(const oscpack::ReceivedBundle& b) const
  {
    using namespace oscpack;
    for(auto i = b.ElementsBegin(); i != b.ElementsEnd(); ++i)
    {
      if(!i->IsBundle())
        on_message(oscpack::ReceivedMessage(*i));
      else
        on_bundle(oscpack::ReceivedBundle(*i));
    }
  }
};
}
