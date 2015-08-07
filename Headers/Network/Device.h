/*!
 * \file Device.h
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

#include "Network/Node.h"

namespace OSSIA
{

class Protocol;

class Device : public virtual Node
{

public:

# pragma mark -
# pragma mark Life cycle

  /*! factory
   \param #Protocol protool parameters
   \param device name
   \return std::shared_ptr<#Device> */
  static std::shared_ptr<Device> create(const Protocol &, std::string = "");

  /*! destructor */
  virtual ~Device() = default;

# pragma mark -
# pragma mark Network

  /*! factory
   \return bool true if the namespace update succeeded */
  virtual bool updateNamespace() = 0;

};

/*! declare Minuit internal device
\note is this still useful ?
\param int in_port where to receive messages
\param int out_port where to send messages
\return std::shared_ptr<Node ??? */
std::shared_ptr<Node> declareMinuitInternDevice(int in_port, int out_port);

/*! declare OSC internal device
\note is this still useful ?
\param int in_port where to receive messages
\param int out_port where to send messages
\return std::shared_ptr<Node ??? */
std::shared_ptr<Node> declareOSCInternDevice(int in_port, int out_port);

}

