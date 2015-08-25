/*!
 * \file OSC.h
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

#include "Network/Protocol.h"

namespace OSSIA
{

class OSC : public virtual Protocol
{
  
public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param std::string ip of the Minuit device
   \param int port where to send messages
   \param int port where messages are sent to
   \return std::shared_ptr<OSC> */
  static std::shared_ptr<OSC> create(std::string, int, int);
  
  /*! destructor */
  virtual ~OSC() = default;
  
# pragma mark -
# pragma mark Accessors
  
  Protocol::Type getType() const override final
  {return Protocol::Type::OSC;}
  
  /*! get IP */
  virtual std::string getIp() = 0;
  
  /*! get port where to send messages */
  virtual int getInPort() = 0;
  
  /*! get port where messages are sent back */
  virtual int getOutPort() = 0;
  
};

}

