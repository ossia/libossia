/*!
 * \file MIDI.h
 *
 * \defgroup Network
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <vector>
#include <string>
#include "Network/Protocol.hpp"

namespace OSSIA
{
class Device;
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

class OSSIA_EXPORT MIDI : public virtual Protocol
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \return std::shared_ptr<MIDI> */
  static std::shared_ptr<MIDI> create();

  /*! destructor */
  virtual ~MIDI();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  virtual MidiInfo getInfo() const = 0;
  virtual bool setInfo(MidiInfo) = 0;

#if 0
# pragma mark -
# pragma mark Operation
#endif

  /*! to see IPs of connected Midi devices
   \todo add options */
  virtual std::vector<MidiInfo> scan() = 0;
};

OSSIA_EXPORT std::shared_ptr<OSSIA::Device> createMIDIDevice(std::shared_ptr<MIDI> mid);

}

