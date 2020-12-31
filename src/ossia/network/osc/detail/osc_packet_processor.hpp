#pragma once
#include <oscpack/osc/OscReceivedElements.h>

namespace ossia::net
{

template<typename F>
struct osc_packet_processor
{
  F on_message;
  void on_bundle( const oscpack::ReceivedBundle& b) const
  {
    using namespace oscpack;
    for (auto i = b.ElementsBegin(); i != b.ElementsEnd(); ++i)
    {
      if(!i->IsBundle())
        on_message(oscpack::ReceivedMessage(*i));
      else
        on_bundle(oscpack::ReceivedBundle(*i));
    }
  }

  void operator()(const oscpack::ReceivedPacket& p) const
  {
    if (!p.IsBundle())
      on_message(oscpack::ReceivedMessage(p));
    else
      on_bundle(oscpack::ReceivedBundle(p));
  }
};


}
