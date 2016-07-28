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

#include <ossia/network/v1/Protocol.hpp>

namespace OSSIA
{
class Device;
class OSSIA_EXPORT OSC : public virtual Protocol
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param std::string ip of the Minuit device
   \param int port where to send messages
   \param int port where messages are sent to
   \return std::shared_ptr<OSC> */
  static std::shared_ptr<OSC> create(std::string, int, int);

  /*! destructor */
  virtual ~OSC();

#if 0
# pragma mark -
# pragma mark Accessors
#endif
  /*! get IP */
  virtual std::string getIp() const = 0;
  virtual Protocol & setIp(std::string) = 0;

  /*! get port where to send messages */
  virtual int getInPort() const = 0;
  virtual Protocol & setInPort(int) = 0;

  /*! get port where messages are sent back */
  virtual int getOutPort() const = 0;
  virtual Protocol & setOutPort(int) = 0;

  /*! enable learning of the addresses */
  virtual bool getLearningStatus() const = 0;
  virtual Protocol & setLearningStatus(OSSIA::Device& ossiaDevice, bool newLearn) = 0;

};

}

