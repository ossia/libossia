/*!
 * \file Minuit.h
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

#include <string>

#include "Network/Protocol.h"
namespace OSSIA
{

struct Minuit : public Protocol
{
  std::string ip;
  int in_port;
  int out_port;
  
# pragma mark -
# pragma mark Life cycle
  
  /*! constructor
   \param std::string ip of the Minuit device
   \param int port where to send messages
   \param int port where messages are sent to */
  Minuit(std::string, int, int);

# pragma mark -
# pragma mark Operation
  
  bool pullAddressValue(std::shared_ptr<Address>) const override;
  
  bool pushAddressValue(std::shared_ptr<Address>) const override;
  
  bool observeAddressValue(std::shared_ptr<Address>, bool) const override;
};

}

