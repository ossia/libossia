#include <Network/Protocol.h>
#include <Network/Protocol/Local.h>
#include <Network/Protocol/MIDI.h>
#include <Network/Protocol/OSC.h>
#include <Network/Protocol/Minuit.h>

namespace OSSIA
{
Protocol::~Protocol() = default;
Local::~Local() = default;
MIDI::~MIDI() = default;
OSC::~OSC() = default;
Minuit::~Minuit() = default;
}
