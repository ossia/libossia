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

#include "Network/Protocol/MIDI.h"

using namespace OSSIA;
using namespace std;

class JamomaMIDI : public MIDI
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaMIDI();
  
  ~JamomaMIDI();
  
# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(shared_ptr<Address>) const override;
  
  bool pushAddressValue(shared_ptr<Address>) const override;
  
  bool observeAddressValue(shared_ptr<Address>, bool) const override;
  
  /*! to see IPs of connected Midi devices
   \todo add options */
  vector<shared_ptr<MIDI>> scan() override;
};
