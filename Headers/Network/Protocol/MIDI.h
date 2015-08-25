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

#include "Network/Protocol.h"

namespace OSSIA
{

struct MIDI : public Protocol
{
  
# pragma mark -
# pragma mark Life cycle

  MIDI();
  
# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(std::shared_ptr<Address>) const override;
  
  bool pushAddressValue(std::shared_ptr<Address>) const override;
  
  bool observeAddressValue(std::shared_ptr<Address>, bool) const override;

# pragma mark -
# pragma mark Specific Operation
  
  /*! to see IPs of connected Midi devices
   \todo add options */
  std::vector<MIDI> scan();
};

}

