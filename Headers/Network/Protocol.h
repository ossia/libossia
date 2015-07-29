/*!
 * \file Protocol.h
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
#include <vector>

namespace OSSIA
{

class Protocol
{
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~Protocol() = default;
  
# pragma mark -
# pragma mark Operation
  
  /*! get the value of an address sending a request
   \details some protocols cannot do this operation
   \return bool true if the operation succeeded */
  //virtual bool getAddressValue(std::shared_ptr<Address>) const = 0;
  
  /*! enable/disable observation on the value of an address sending a request
   \details some protocols cannot do this operation
   \return bool true if the operation succeeded */
  //virtual bool observeAddressValue(std::shared_ptr<Address>, bool) const = 0;
  
};

# pragma mark -
# pragma mark Local
  
struct Local : public Protocol {};

# pragma mark -
# pragma mark Minuit
  
struct Minuit : public Protocol
{
  /*! constructor 
   \param std::string ip of the Minuit device
   \param int port where to send messages
   \param int port where messages are sent to */
  Minuit(std::string ip, int in_port, int out_port)
    :ip(ip), in_port(in_port), out_port(out_port) {}

  std::string ip;
  int in_port;
  int out_port;
};

# pragma mark -
# pragma mark OSC

struct OSC : public Protocol
{
  /*! constructor
   \param std::string ip of the OSC device
   \param int port where to send messages
   \param int port where messages are sent to */
  OSC(std::string ip, int in_port, int out_port)
    :ip(ip), in_port(in_port), out_port(out_port) {}

  std::string ip;
  int in_port;
  int out_port;
};
  
# pragma mark -
# pragma mark Midi

struct Midi : public Protocol
{
  /*! to see IPs of connected Midi devices
   \todo add options */
  static std::vector<Midi> scan();
};

}

