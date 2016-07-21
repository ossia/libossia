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

#include "Network/Protocol.h"
namespace OSSIA
{

class OSSIA_EXPORT Minuit : public virtual Protocol
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param std::string ip of the Minuit device
   \param int port where to send messages
   \param int port where messages are sent back
   \return std::shared_ptr<Minuit> */
  static std::shared_ptr<Minuit> create(std::string, int, int);

  /*! destructor */
  virtual ~Minuit();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! get IP */
  virtual std::string getIp() = 0;
  virtual Protocol & setIp(std::string) = 0;

  /*! get port where to send messages */
  virtual int getInPort() = 0;
  virtual Protocol & setInPort(int) = 0;

  /*! get port where messages are sent back */
  virtual int getOutPort() = 0;
  virtual Protocol & setOutPort(int) = 0;

};

}

