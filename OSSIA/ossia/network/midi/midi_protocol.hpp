#pragma once
#include <ossia/network/domain.hpp>
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

namespace OSSIA
{
namespace net
{
struct OSSIA_EXPORT MidiInfo
{
    enum class Type { RemoteInput, RemoteOutput };

    MidiInfo() = default;
    MidiInfo(Type t, std::string d, int p):
      type{t},
      device{std::move(d)},
      port{p}
    {

    }

    Type type{};
    std::string device{};
    int port{};
};

class OSSIA_EXPORT MIDI final :
    public OSSIA::net::Protocol
{
    mm::MidiInput mInput;
    mm::MidiOutput mOutput;

    std::array<Channel, 16> mChannels;

    MidiInfo mInfo;

  public:
    MIDI();
    ~MIDI();

    bool setInfo(MidiInfo);
    MidiInfo getInfo() const;

    bool pull(OSSIA::net::Address&) override;
    bool push(const OSSIA::net::Address&) override;
    bool observe(OSSIA::net::Address&, bool) override;
    bool update(OSSIA::net::Node& node) override;

    std::vector<MidiInfo> scan();
};

}
}
