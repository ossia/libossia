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
#include "Network/Protocol.h"

namespace OSSIA
{
class Device;
struct MidiInfo
{
        enum class Type { Input, Output };

        MidiInfo(Type t, std::string d, int p):
            type{t},
            device{std::move(d)},
            port{p}
        {

        }

        Type type;
        std::string device;
        int port;
};

class MIDI : public virtual Protocol
{

public:

# pragma mark -
# pragma mark Life cycle

  /*! factory
   \return std::shared_ptr<MIDI> */
  static std::shared_ptr<MIDI> create();

  /*! destructor */
  virtual ~MIDI();

# pragma mark -
# pragma mark Accessors

  Protocol::Type getType() const override final
  {return Protocol::Type::MIDI;}

# pragma mark -
# pragma mark Operation

  /*! to see IPs of connected Midi devices
   \todo add options */
  virtual std::vector<MidiInfo> scan() = 0;
};

std::shared_ptr<OSSIA::Device> createMIDIDevice(std::shared_ptr<MIDI> mid);

}

