/*!
 * \file JamomaMIDI.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include <ossia/network/Domain.hpp>
#include <ossia/network/base/Protocol.hpp>
#include <ossia/network/base/Device.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/network/base/Address.hpp>
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

class MIDI final :
    public OSSIA::net::Protocol
{
    mm::MidiInput mInput;
    mm::MidiOutput mOutput;

    std::array<Channel, 16> mChannels;

    MidiInfo mInfo;
    bool setInfo(MidiInfo);
    MidiInfo getInfo() const;

  public:
    MIDI();
    ~MIDI();

    bool pull(OSSIA::net::Address&) override;
    bool push(const OSSIA::net::Address&) override;
    bool observe(OSSIA::net::Address&, bool) override;
    bool update(OSSIA::net::Node& node) override;

    std::vector<MidiInfo> scan();
};

}
}
