#pragma once
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <cassert>
#include <array>

#include <ModernMIDI/midi_input.h>
#include <ModernMIDI/midi_output.h>
#include <atomic>

namespace ossia
{
namespace net
{
namespace midi
{
struct OSSIA_EXPORT midi_info
{
    enum class Type { RemoteInput, RemoteOutput };

    midi_info() = default;
    midi_info(Type t, std::string d, int p):
      type{t},
      device{std::move(d)},
      port{p}
    {

    }

    Type type{};
    std::string device{};
    int port{};
};

class OSSIA_EXPORT midi_protocol final :
    public ossia::net::protocol_base
{
    mm::MidiInput mInput;
    mm::MidiOutput mOutput;

    std::array<midi_channel, 16> mChannels;

    midi_info mInfo;

  public:
    midi_protocol();
    ~midi_protocol();

    bool setInfo(midi_info);
    midi_info getInfo() const;

    bool pull(ossia::net::address_base&) override;
    bool push(const ossia::net::address_base&) override;
    bool observe(ossia::net::address_base&, bool) override;
    bool update(ossia::net::node_base& node_base) override;

    std::vector<midi_info> scan();
};

}
}
}
