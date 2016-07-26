#include  <ossia/network/v1/Protocol.hpp>
#include  <ossia/network/v1/Protocol/Local.hpp>
#include  <ossia/network/v1/Protocol/MIDI.hpp>
#include  <ossia/network/v1/Protocol/Minuit.hpp>
#include  <ossia/network/v1/Protocol/OSC.hpp>

namespace OSSIA
{
Protocol::~Protocol() = default;
Local::~Local() = default;
MIDI::~MIDI() = default;
OSC::~OSC() = default;
Minuit::~Minuit() = default;
}
